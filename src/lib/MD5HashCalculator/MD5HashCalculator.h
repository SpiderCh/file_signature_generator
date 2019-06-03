#ifndef MD5_HASH_CALCULATOR_H
#define MD5_HASH_CALCULATOR_H

#include "IHashCalculator.h"

#ifdef __APPLE__
#define DLL_EXPORT
#else
#define DLL_EXPORT __declspec(dllexport)
#endif

namespace Hash
{
class DLL_EXPORT MD5Hash : public IHashCalculator
{
public:
	std::string CalculateHash(const std::vector<std::uint8_t> & data) override;
};
} // namespace Hash

#undef DLL_EXPORT

#endif
