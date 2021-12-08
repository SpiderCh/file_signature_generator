#ifndef IFSTREAM_DATA_PROVIDER_H
#define IFSTREAM_DATA_PROVIDER_H

#include <string>
#include <fstream>
#include <vector>

#include "IDataProvider.h"

class IFStreamDataProvider : public IDataProvider
{

public:
	IFStreamDataProvider(const std::string & filePath);
	~IFStreamDataProvider();

	size_t Read(size_t from, size_t bytes) override;
	const std::uint8_t * Data() const override;
	std::size_t TotalSize() const override;
	bool Eof() override;

private:
	const std::string m_filePath;
	const size_t m_fileSize;
	std::vector<std::uint8_t> m_data;

	std::ifstream m_fileStream;
};

#endif
