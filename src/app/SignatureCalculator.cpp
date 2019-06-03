#include "SignatureCalculator.h"

#include <future>
#include <thread>
#include <vector>
#include <iostream>
#include <mutex>
#include <atomic>
#include <optional>
#include <condition_variable>

#include "IDataProvider.h"
#include "IDataProviderFactory.h"
#include "IHashCalculator.h"

namespace Calculator
{

struct CalculatorManager::Impl
{
	const std::shared_ptr<IDataProviderFactory> data_provider_factory;
	const std::shared_ptr<Hash::IHashCalculator> hash_calculator;
	const size_t bytes_to_read;
	const unsigned int num_of_available_threads;

	std::atomic_bool stop_execution {false};

	std::vector<std::thread> threads_pool;
	std::vector<std::mutex> threads_mutex;
	std::vector<std::condition_variable> threads_conditional_variables;
	std::vector<std::optional<std::packaged_task<std::string()>>> thread_tasks_pool;

	Impl(const std::shared_ptr<IDataProviderFactory> & data_provider_factory,
		 const std::shared_ptr<Hash::IHashCalculator> & hash_calculator,
		 const size_t read_size)
		: data_provider_factory(data_provider_factory)
		, hash_calculator(hash_calculator)
		, bytes_to_read(read_size)
		, num_of_available_threads(std::thread::hardware_concurrency() > 0 ? std::thread::hardware_concurrency() : 1)
		, threads_mutex(num_of_available_threads)
		, threads_conditional_variables(num_of_available_threads)
		, thread_tasks_pool(num_of_available_threads)
	{
		assert(data_provider_factory);
		assert(hash_calculator);
		assert(bytes_to_read > 0);

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
				assert(false);

			data_provider->thread_tasks_pool[thread_index].value()();
			data_provider->thread_tasks_pool[thread_index] = std::nullopt;
		}
	}
};

CalculatorManager::CalculatorManager(const std::shared_ptr<IDataProviderFactory> & data_provider_factory, const std::shared_ptr<Hash::IHashCalculator> & hash_calculator, const size_t read_size)
	: m_impl(std::make_unique<Impl>(data_provider_factory, hash_calculator, read_size))
{}

CalculatorManager::~CalculatorManager() = default;

void CalculatorManager::Start()
{
	size_t size = 0;
	std::unique_ptr<IDataProvider> data_provider = m_impl->data_provider_factory->CreateDataProvider();
	while(!data_provider->eof())
	{
		std::vector<std::future<std::string>> workers;

		for (unsigned int i = 0; i < m_impl->num_of_available_threads; ++i)
		{
			std::vector<std::uint8_t> data = data_provider->Read(m_impl->bytes_to_read);
			size += data.size();

			std::lock_guard<std::mutex> lock(m_impl->threads_mutex[i]);
			std::packaged_task<std::string()> task([data, this]()
			{
				const std::string result = m_impl->hash_calculator->CalculateHash(data);
				return result;
			});

			workers.push_back(std::move(task.get_future()));
			m_impl->thread_tasks_pool[i] = std::move(task);

			if (data_provider->eof())
				break;
		}

		for (size_t i = 0; i < workers.size(); ++i)
			m_impl->threads_conditional_variables[i].notify_all();

		const size_t data_size = workers.size();
		std::cout << "Overall size: " << size << std::endl;
		for (size_t i = 0; i < data_size; ++i)
		{
			assert(workers[i].valid());
			const std::string result = workers[i].get();
			std::cout << "Hash Result: " << result << std::endl;
		}
	}
}

} // namespace Calculator

