#ifndef MD5_HASH_CALCULATOR_H
#define MD5_HASH_CALCULATOR_H

#include "IHashCalculator.h"

namespace Hash
{
class MD5Hash : public IHashCalculator
{
public:
	std::string CalculateHash(std::vector<char> data) override;
};
} // namespace Hash

#endif
