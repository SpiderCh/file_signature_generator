#include "SignatureCalculator.h"

#include <future>
#include <thread>
#include <vector>
#include <iostream>
#include <mutex>
#include <atomic>
#include <optional>
#include <cassert>
#include <condition_variable>

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

struct CalculatorManager::Impl
{
	const std::shared_ptr<IDataProviderFactory> data_provider_factory;
	const std::shared_ptr<IHashSaver> hash_saver;
	const std::shared_ptr<Hash::IHashCalculator> hash_calculator;
	const size_t bytes_to_read;
	unsigned int num_of_available_threads;

	std::atomic_bool stop_execution {false};

	std::vector<std::thread> threads_pool;
	std::vector<std::mutex> threads_mutex;
	std::vector<std::condition_variable> threads_conditional_variables;
	std::vector<std::unique_ptr<IDataProvider>> thread_data_providers;
	std::vector<std::optional<std::packaged_task<std::string()>>> thread_tasks_pool;

	Impl(const std::shared_ptr<IDataProviderFactory> & data_provider_factory,
		 const std::shared_ptr<IHashSaver> & hash_saver,
		 const std::shared_ptr<Hash::IHashCalculator> & hash_calculator,
		 const std::string & filePath,
		 const size_t read_size)
		: data_provider_factory(data_provider_factory)
		, hash_saver(hash_saver)
		, hash_calculator(hash_calculator)
		, bytes_to_read(read_size)
		, num_of_available_threads(std::thread::hardware_concurrency() > 0 ? std::thread::hardware_concurrency() : 1)
		, threads_mutex(num_of_available_threads)
		, threads_conditional_variables(num_of_available_threads)
		, thread_tasks_pool(num_of_available_threads)
	{
		assert(data_provider_factory);
		assert(hash_saver);
		assert(hash_calculator);
		assert(bytes_to_read > 0);

		const size_t fileSize = boost::filesystem::file_size(filePath);
		if (fileSize / bytes_to_read < num_of_available_threads)
			num_of_available_threads = fileSize / bytes_to_read;

		for (unsigned int i = 0; i < num_of_available_threads; ++i)
			threads_pool.emplace_back(&Impl::ThreadWorker, i, this);
	}

	~Impl()
	{
		stop_execution = true;
		for (unsigned int i = 0; i < num_of_available_threads; ++i)
		{
			std::lock_guard<std::mutex> lock(threads_mutex[i]);
			threads_conditional_variables[i].notify_all();
		}

		for (unsigned int i = 0; i < num_of_available_threads; ++i)
		{
			if (threads_pool[i].joinable())
				threads_pool[i].join();
		}
	}

	static void ThreadWorker(int thread_index, Impl* data_provider)
	{
		std::mutex & thread_mutex = data_provider->threads_mutex[thread_index];
		std::condition_variable & thread_conditional_variable = data_provider->threads_conditional_variables[thread_index];

		while (true)
		{
			std::unique_lock<std::mutex> lock(thread_mutex);
			thread_conditional_variable.wait(lock);

			if (data_provider->stop_execution)
				break;

			if (!data_provider->thread_tasks_pool[thread_index].has_value())
				continue;

			data_provider->thread_tasks_pool[thread_index].value()();
			data_provider->thread_tasks_pool[thread_index] = std::nullopt;
		}
	}
};

CalculatorManager::CalculatorManager(const std::shared_ptr<IDataProviderFactory> & data_provider_factory,
									 const std::shared_ptr<IHashSaver> & hash_saver,
									 const std::shared_ptr<Hash::IHashCalculator> & hash_calculator,
									 const std::string & filePath,
									 const size_t read_size)
	: m_filePath(filePath)
	, m_impl(std::make_unique<Impl>(data_provider_factory, hash_saver, hash_calculator, filePath, read_size))
{}

CalculatorManager::~CalculatorManager() = default;

void CalculatorManager::Start()
{
	int fileDescriptor = open(m_filePath.data(), O_RDONLY);

	if (fileDescriptor < 0)
		throw std::runtime_error("Cannot open file: " + m_filePath);

	const size_t fileSize = boost::filesystem::file_size(m_filePath);

//	std::cerr << "File size: " << fileSize << std::endl;
//	std::cerr << "bytes_to_read: " << m_impl->bytes_to_read << std::endl;
	for (size_t iteration = 0; ; ++iteration)
	{
		const size_t readFrom = iteration * m_impl->bytes_to_read * m_impl->num_of_available_threads;
		size_t bytesToRead = m_impl->bytes_to_read * m_impl->num_of_available_threads;
		size_t numOfThreads = m_impl->num_of_available_threads;

		if (readFrom > fileSize)
			break;

		if (bytesToRead > fileSize)
		{
			bytesToRead = fileSize - readFrom;
			numOfThreads = bytesToRead / m_impl->bytes_to_read;
		}

//		std::cerr << "  Fixed bytes to read: " << bytesToRead << " Original Bytes To Read: " << m_impl->bytes_to_read << std::endl;

		void * dataPtr = mmap(NULL, bytesToRead, PROT_READ, MAP_SHARED, fileDescriptor, readFrom);

		std::vector<std::future<std::string>> workers;
		for (unsigned int i = 0; i < numOfThreads; ++i)
		{
			std::lock_guard<std::mutex> lock(m_impl->threads_mutex[i]);
			std::packaged_task<std::string()> task([this, dataPtr, readFrom, i, bytesToRead]() -> std::string
			{
				size_t dataSize = m_impl->bytes_to_read;
//				if (bytesToRead - m_impl->bytes_to_read * processedBlocks <= 0)
//					dataSize = bytesToRead / processedBlocks;

				return m_impl->hash_calculator->CalculateHash(reinterpret_cast<uint8_t *>(dataPtr) + m_impl->bytes_to_read * i, dataSize);
			});

			workers.push_back(std::move(task.get_future()));
			m_impl->thread_tasks_pool[i] = std::move(task);
		}

		for (std::condition_variable & var : m_impl->threads_conditional_variables)
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

			m_impl->hash_saver->Save(result.value());
		}

		munmap(dataPtr, bytesToRead);

		if (work_finished)
			break;
	}

	m_impl->thread_data_providers.clear();
	close(fileDescriptor);
}

} // namespace Calculator
