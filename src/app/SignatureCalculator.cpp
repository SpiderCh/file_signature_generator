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
	const unsigned int num_of_available_threads;

	std::atomic_bool stop_execution {false};

	std::vector<std::thread> threads_pool;
	std::vector<std::mutex> threads_mutex;
	std::vector<std::condition_variable> threads_conditional_variables;
	std::vector<std::unique_ptr<IDataProvider>> thread_data_providers;
	std::vector<std::optional<std::packaged_task<std::optional<std::string>(const int)>>> thread_tasks_pool;

	Impl(const std::shared_ptr<IDataProviderFactory> & data_provider_factory,
		 const std::shared_ptr<IHashSaver> & hash_saver,
		 const std::shared_ptr<Hash::IHashCalculator> & hash_calculator,
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
			{
				assert(false);
				std::cerr << "Thread woken with invalid task: " << thread_index << std::endl;
				continue;
			}

			data_provider->thread_tasks_pool[thread_index].value()(thread_index);
			data_provider->thread_tasks_pool[thread_index] = std::nullopt;
		}
	}
};

CalculatorManager::CalculatorManager(const std::shared_ptr<IDataProviderFactory> & data_provider_factory,
									 const std::shared_ptr<IHashSaver> & hash_saver,
									 const std::shared_ptr<Hash::IHashCalculator> & hash_calculator,
									 const size_t read_size)
	: m_impl(std::make_unique<Impl>(data_provider_factory, hash_saver, hash_calculator, read_size))
{}

CalculatorManager::~CalculatorManager() = default;

void CalculatorManager::Start()
{
	try
	{
		for (unsigned int i = 0; i < m_impl->num_of_available_threads; ++i)
			m_impl->thread_data_providers.emplace_back(std::move(m_impl->data_provider_factory->CreateDataProvider()));
	}
	catch (const std::exception & ex)
	{
		std::cerr << "Error during initialization of readers: " << ex.what() << std::endl;
		return;
	}

	unsigned int processed_blocks = 0;
	while (true)
	{
		std::vector<std::future<std::optional<std::string>>> workers;
		for (unsigned int i = 0; i < m_impl->num_of_available_threads; ++i)
		{
			std::lock_guard<std::mutex> lock(m_impl->threads_mutex[i]);
			std::packaged_task<std::optional<std::string>(const int)> task([this, processed_blocks](const int thread_id) -> std::optional<std::string>
			{
				const size_t read_from = processed_blocks * m_impl->bytes_to_read;
				if (m_impl->thread_data_providers[thread_id]->eof())
					return std::nullopt;

				const std::vector<std::uint8_t> data = m_impl->thread_data_providers[thread_id]->Read(read_from, m_impl->bytes_to_read);
				if (data.empty())
					return std::nullopt;

				const std::string result = m_impl->hash_calculator->CalculateHash(data);
				return std::make_optional(result);
			});

			++processed_blocks;
			workers.push_back(std::move(task.get_future()));
			m_impl->thread_tasks_pool[i] = std::move(task);
		}

		for (std::condition_variable & var : m_impl->threads_conditional_variables)
			var.notify_all();

		bool work_finished = workers.empty();
		for (std::future<std::optional<std::string>> & worker : workers)
		{
			std::optional<std::string> result = worker.get();
			if (!result)
			{
				work_finished = true;
				continue;
			}

			m_impl->hash_saver->Save(result.value());
		}

		if (work_finished)
			break;
	}

	m_impl->thread_data_providers.clear();
}

} // namespace Calculator
