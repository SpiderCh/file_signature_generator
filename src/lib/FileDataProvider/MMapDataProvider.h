#ifndef MMAP_DATA_PROVIDER_H
#define MMAP_DATA_PROVIDER_H

#include <string>

#include "IDataProvider.h"

class MMapDataProvider : public IDataProvider
{

public:
	MMapDataProvider(const std::string & filePath);
	~MMapDataProvider();

	size_t Read(size_t from, size_t bytes) override;
	const std::uint8_t * Data() const override;
	std::size_t TotalSize() const override;
	bool Eof() override;

private:
	const std::string m_filePath;
	const int m_fileDescriptor;
	const size_t m_fileSize;
	bool m_eof = false;

	void * m_dataPtr = nullptr;
	size_t m_previouslyReadBytesSize = 0;
};

#endif
