#ifndef CRC_HASH_CALCULATOR_H
#define CRC_HASH_CALCULATOR_H

#include "IHashCalculator.h"

namespace Hash
{
class CRCHash : public IHashCalculator
{
public:
	std::string CalculateHash(const std::vector<std::uint8_t> & data) override;
};
} // namespace Hash

#endif
