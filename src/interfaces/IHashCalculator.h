#ifndef IHASH_CALCULATOR_H
#define IHASH_CALCULATOR_H

#include <string>
#include <vector>

namespace Hash
{

class IHashCalculator
{
public:
	virtual ~IHashCalculator() = default;
	virtual std::string CalculateHash(const std::vector<std::uint8_t> & data) = 0;
	virtual std::string CalculateHash(const std::uint8_t * data, size_t size) = 0;
};
} // namespace Hash

#endif
