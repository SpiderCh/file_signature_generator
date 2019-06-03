#ifndef SIGNATURE_CALCULATOR_H
#define SIGNATURE_CALCULATOR_H

#include <memory>

class IHashSaver;
class IDataProviderFactory;

namespace Hash { class IHashCalculator; }

namespace Calculator
{

class CalculatorManager
{
public:
	CalculatorManager(const std::shared_ptr<IDataProviderFactory> & data_provider,
					  const std::shared_ptr<IHashSaver> & hash_saver,
					  const std::shared_ptr<Hash::IHashCalculator> & hash_calculator,
					  const size_t read_size);

	~CalculatorManager();
	void Start();

private:
	struct Impl;
	std::unique_ptr<Impl> m_impl;
};
} // namespace Calculator

#endif
