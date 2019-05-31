#ifndef IDATA_PROVIDER_H
#define IDATA_PROVIDER_H

#include <vector>

class IDataProvider
{
public:
	virtual ~IDataProvider() = default;

	virtual bool Initialize() = 0;
	virtual std::vector<std::uint8_t> Read(size_t bytes) = 0;
	virtual bool eof() = 0;
};

#endif
