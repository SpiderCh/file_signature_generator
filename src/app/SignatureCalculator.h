#ifndef SIGNATURE_CALCULATOR_H
#define SIGNATURE_CALCULATOR_H

#include <memory>
#include <string>

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
					  const std::string & filePath,
					  const size_t read_size);

	~CalculatorManager();
	void Start();

private:
	const std::string m_filePath;
	struct Impl;
	std::unique_ptr<Impl> m_impl;
};
} // namespace Calculator

#endif
