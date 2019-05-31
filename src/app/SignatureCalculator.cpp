#include "SignatureCalculator.h"

#include <future>
#include <thread>
#include <vector>
#include <iostream>

#include "IDataProvider.h"
#include "IHashCalculator.h"

namespace Calculator
{

struct CalculatorManager::Impl
{
	const std::shared_ptr<IDataProvider> data_provider;
	const std::shared_ptr<Hash::IHashCalculator> hash_calculator;
	const size_t bytes_to_read;

	Impl(const std::shared_ptr<IDataProvider> & data_provider,
		 const std::shared_ptr<Hash::IHashCalculator> & hash_calculator,
		 const size_t read_size)
		: data_provider(data_provider)
		, hash_calculator(hash_calculator)
		, bytes_to_read(read_size)
	{
		assert(data_provider);
		assert(hash_calculator);
		assert(bytes_to_read > 0);
	}
};

CalculatorManager::CalculatorManager(const std::shared_ptr<IDataProvider> & data_provider, const std::shared_ptr<Hash::IHashCalculator> & hash_calculator, const size_t read_size)
	: m_impl(std::make_unique<Impl>(data_provider, hash_calculator, read_size))
{}

CalculatorManager::~CalculatorManager() = default;

void CalculatorManager::Start()
{
	size_t size = 0;
	const unsigned int hardware_concurrency = std::thread::hardware_concurrency();
	while(!m_impl->data_provider->eof())
	{
		std::vector<std::future<std::string>> workers;

		for (unsigned int i = 0; i < hardware_concurrency; ++i)
		{
			std::vector<std::uint8_t> data = m_impl->data_provider->Read(m_impl->bytes_to_read);
			size += data.size();

			std::packaged_task<std::string()> task([data, this]
			{
				const std::string result = m_impl->hash_calculator->CalculateHash(data);
				return result;
			});
			workers.push_back(std::move(task.get_future()));
			std::thread(std::move(task)).detach();
			assert(workers.back().valid());

			if (m_impl->data_provider->eof())
				break;
		}

		for (std::future<std::string> & worker : workers)
			worker.wait();

		const size_t data_size = workers.size();
		std::cout << "Overall size: " << size;
		for (size_t i = 0; i < data_size; ++i)
		{
			assert(workers[i].valid());
			const std::string result = workers[i].get();
			std::cout << "Hash Result: " << result << std::endl;
		}
	}
}

} // namespace Calculator

