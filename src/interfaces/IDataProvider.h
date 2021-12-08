#ifndef IDATA_PROVIDER_H
#define IDATA_PROVIDER_H

#include <vector>

class IDataProvider
{
public:
	virtual ~IDataProvider() = default;

	/// @brief Reads n bytes from desired position
	/// @note May throw exception
	/// @return size of read data
	virtual size_t Read(size_t from, size_t bytes) = 0;
	/// @brief Return pointer to the begin of read data
	virtual std::uint8_t * Data() const = 0;
	/// @brief Return total size of source.
	virtual std::size_t TotalSize() const = 0;
	virtual bool Eof() = 0;
};

#endif
