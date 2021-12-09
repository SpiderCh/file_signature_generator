#include "SignatureCalculator.h"

#include "IHashSaver.h"
#include "IDataProvider.h"
#include "IHashCalculator.h"

#if __x86_64__ || __ppc64__ || __arm64__ || _WIN64
	#define ENV64BIT
#else
	#define ENV32BIT
#endif

namespace Calculator
{

namespace
{
unsigned int CalculateNumberOfAvailableThreads(const size_t fileSize, const size_t bytesToRead)
{
	if (bytesToRead < 1)
		throw std::invalid_argument("Invalid bytes to read value.");

	if (std::thread::hardware_concurrency() < 2)
		return 1;
	unsigned int numberOfAvailableThreads = std::thread::hardware_concurrency();
	if (fileSize / numberOfAvailableThreads < bytesToRead)
		numberOfAvailableThreads = fileSize / bytesToRead;

#ifdef ENV32BIT
	constexpr size_t FOUR_GB_IN_BYTES = 4294967296;
	if (bytesToRead * numberOfAvailableThreads >= FOUR_GB_IN_BYTES)
	{
		for(; numberOfAvailableThreads > 1; --numberOfAvailableThreads)
			if (bytesToRead * numberOfAvailableThreads < FOUR_GB_IN_BYTES)
				break;
	}
#endif

	return numberOfAvailableThreads;
}
}

CalculatorManager::CalculatorManager(const std::shared_ptr<IDataProvider> & dataProvider,
									 const std::shared_ptr<IHashSaver> & hashSaver,
									 const std::shared_ptr<Hash::IHashCalculator> & hashCalculator,
									 const size_t readSize)
	: m_dataProvider(dataProvider)
	, m_hashSaver(hashSaver)
	, m_hashCalculator(hashCalculator)
	, m_bytesToRead(readSize)
	, m_numberOfAvailableThreads(CalculateNumberOfAvailableThreads(m_dataProvider->TotalSize(), readSize))
	, m_threadsMutexes(m_numberOfAvailableThreads)
	, m_threadsConditionalVariables(m_numberOfAvailableThreads)
	, m_threadsTasksPool(m_numberOfAvailableThreads)
{
	if (!m_dataProvider)
		throw std::invalid_argument("Invalid data provider.");
	if (!m_hashSaver)
		throw std::invalid_argument("Invalid hash saver.");
	if (!m_hashCalculator)
		throw std::invalid_argument("Invalid hash calculator.");

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
	std::vector<std::future<std::string>> workers;
	workers.resize(m_numberOfAvailableThreads);
	for (size_t iteration = 0; ; ++iteration)
	{
		const size_t readFrom = iteration * m_bytesToRead * m_numberOfAvailableThreads;
		const size_t bytesToRead = m_bytesToRead * m_numberOfAvailableThreads;

		const size_t readBytes = m_dataProvider->Read(readFrom, bytesToRead);

		if (readBytes == 0)
			break;

		size_t numOfThreads = m_numberOfAvailableThreads;
		if (bytesToRead != readBytes)
			numOfThreads = readBytes / m_bytesToRead;

		for (unsigned int i = 0; i < numOfThreads; ++i)
		{
			std::lock_guard<std::mutex> lock(m_threadsMutexes[i]);
			std::packaged_task<std::string()> task([this, readFrom, i]()
			{
				size_t dataSize = m_bytesToRead;
				if (dataSize > m_dataProvider->TotalSize() - (readFrom + dataSize * i))
					dataSize = m_dataProvider->TotalSize() - (readFrom + dataSize * i);

				return m_hashCalculator->CalculateHash(m_dataProvider->Data() + m_bytesToRead * i, dataSize);
			});

			workers[i] = std::move(task.get_future());
			m_threadsTasksPool[i] = std::move(task);
			m_threadsConditionalVariables[i].notify_all();
		}

		for (size_t i = 0; i < numOfThreads; ++i)
			m_hashSaver->Save(workers[i].get());
	}
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

		m_threadsTasksPool[threadIndex]();
	}
}

} // namespace Calculator
