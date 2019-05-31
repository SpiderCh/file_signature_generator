#ifndef MD5_HASH_CALCULATOR_H
#define MD5_HASH_CALCULATOR_H

#include "IHashCalculator.h"

namespace Hash
{
class MD5Hash : public IHashCalculator
{
public:
	std::string CalculateHash(const std::vector<std::uint8_t> & data) override;
};
} // namespace Hash

#endif
