#include "SignatureCalculator.h"

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
	while(!m_impl->data_provider->eof())
	{
		const std::vector<char> data = m_impl->data_provider->Read(m_impl->bytes_to_read);
		if (data.empty())
			break;

		size += data.size();
		const std::string result = m_impl->hash_calculator->CalculateHash(data);
		std::cout << "Read: " << data.size() << " Overall size: " << size << " Result: " << result <<std::endl;
	}
}

} // namespace Calculator

