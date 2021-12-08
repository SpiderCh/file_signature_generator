#include "SignatureCalculator.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include <boost/filesystem.hpp>

#include "IHashSaver.h"
#include "IDataProvider.h"
#include "IDataProviderFactory.h"
#include "IHashCalculator.h"

namespace Calculator
{

CalculatorManager::CalculatorManager(const std::shared_ptr<IHashSaver> & hashSaver,
									 const std::shared_ptr<Hash::IHashCalculator> & hashCalculator,
									 const std::string & filePath,
									 const size_t readSize)
	: m_filePath(filePath)
	, m_hashSaver(hashSaver)
	, m_hashCalculator(hashCalculator)
	, m_bytesToRead(readSize)
	, m_numberOfAvailableThreads(std::thread::hardware_concurrency() > 0 ? std::thread::hardware_concurrency() : 1)
	, m_threadsMutexes(m_numberOfAvailableThreads)
	, m_threadsConditionalVariables(m_numberOfAvailableThreads)
	, m_threadsTasksPool(m_numberOfAvailableThreads)
{
	if (!m_hashSaver)
		throw std::invalid_argument("Invalid hash saver.");
	if (!m_hashCalculator)
		throw std::invalid_argument("Invalid hash calculator.");
	if (m_bytesToRead < 1)
		throw std::invalid_argument("Invalid bytes to read value.");

	for (unsigned int i = 0; i < m_numberOfAvailableThreads; ++i)
		m_threadsPool.emplace_back(&CalculatorManager::ThreadWorker, this, i);
}

CalculatorManager::~CalculatorManager()
{
	m_stopExecution = true;
	for (unsigned int i = 0; i < m_numberOfAvailableThreads; ++i)
	{
		std::lock_guard<std::mutex> lock(m_threadsMutexes[i]);
		m_threadsConditionalVariables[i].notify_all();
	}

	for (unsigned int i = 0; i < m_numberOfAvailableThreads; ++i)
	{
		if (m_threadsPool[i].joinable())
			m_threadsPool[i].join();
	}
}

void CalculatorManager::Start()
{
	int fileDescriptor = open(m_filePath.data(), O_RDONLY);

	if (fileDescriptor < 0)
		throw std::runtime_error("Cannot open file: " + m_filePath);

	const size_t fileSize = boost::filesystem::file_size(m_filePath);

	for (size_t iteration = 0; ; ++iteration)
	{
		const size_t readFrom = iteration * m_bytesToRead * m_numberOfAvailableThreads;
		size_t bytesToRead = m_bytesToRead * m_numberOfAvailableThreads;
		size_t numOfThreads = m_numberOfAvailableThreads;

		if (readFrom > fileSize)
			break;

		if (bytesToRead > fileSize)
		{
			bytesToRead = fileSize - readFrom;
			numOfThreads = bytesToRead / m_bytesToRead;
		}

		void * dataPtr = mmap(NULL, bytesToRead, PROT_READ, MAP_SHARED, fileDescriptor, readFrom);

		std::vector<std::future<std::string>> workers;
		workers.resize(numOfThreads);
		for (unsigned int i = 0; i < numOfThreads; ++i)
		{
			std::lock_guard<std::mutex> lock(m_threadsMutexes[i]);
			std::packaged_task<std::string()> task([this, dataPtr, readFrom, i, bytesToRead]() -> std::string
			{
				size_t dataSize = m_bytesToRead;
//				if (bytesToRead - m_impl->bytes_to_read * processedBlocks <= 0)
//					dataSize = bytesToRead / processedBlocks;

				return m_hashCalculator->CalculateHash(reinterpret_cast<uint8_t *>(dataPtr) + m_bytesToRead * i, dataSize);
			});

			workers[i] = std::move(task.get_future());
			m_threadsTasksPool[i] = std::move(task);
		}

		for (std::condition_variable & var : m_threadsConditionalVariables)
			var.notify_all();

		bool work_finished = workers.empty();
		for (std::future<std::string> & worker : workers)
		{
			std::optional<std::string> result = worker.get();
			if (!result)
			{
				work_finished = true;
				continue;
			}

			m_hashSaver->Save(result.value());
		}

		munmap(dataPtr, bytesToRead);

		if (work_finished)
			break;
	}

	close(fileDescriptor);
}

void CalculatorManager::ThreadWorker(int threadIndex)
{
	std::mutex & thread_mutex = m_threadsMutexes[threadIndex];
	std::condition_variable & thread_conditional_variable = m_threadsConditionalVariables[threadIndex];

	while (true)
	{
		std::unique_lock<std::mutex> lock(thread_mutex);
		thread_conditional_variable.wait(lock);

		if (m_stopExecution)
			break;

		if (!m_threadsTasksPool[threadIndex].has_value())
			continue;

		m_threadsTasksPool[threadIndex].value()();
		m_threadsTasksPool[threadIndex] = std::nullopt;
	}
}

} // namespace Calculator
