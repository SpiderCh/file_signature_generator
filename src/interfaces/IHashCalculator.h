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
	virtual std::string CalculateHash(std::vector<unsigned char> data) = 0;
};
} // namespace Hash

#endif
