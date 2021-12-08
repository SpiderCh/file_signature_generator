#include "MMapDataProvider.h"

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include <boost/filesystem.hpp>

MMapDataProvider::MMapDataProvider(const std::string & filePath)
	: m_filePath(filePath)
	, m_fileDescriptor(open(m_filePath.data(), O_RDONLY))
	, m_fileSize(boost::filesystem::file_size(m_filePath))
{}

MMapDataProvider::~MMapDataProvider()
{
	if (m_dataPtr != nullptr)
		munmap(m_dataPtr, m_previouslyReadBytesSize);
	close(m_fileDescriptor);
}

size_t MMapDataProvider::Read(size_t from, size_t bytes)
{
	if (from > m_fileSize)
	{
		m_eof = true;
		return 0;
	}

	if (bytes > m_fileSize - from)
		bytes = m_fileSize - from;

	if (m_dataPtr != nullptr)
		munmap(m_dataPtr, m_previouslyReadBytesSize);

	errno = 0;
	m_previouslyReadBytesSize = bytes;
	m_dataPtr = mmap(nullptr, bytes, PROT_READ, MAP_SHARED, m_fileDescriptor, from);

	if (errno != 0 || m_dataPtr == MAP_FAILED)
	{
		m_dataPtr = nullptr;
		m_previouslyReadBytesSize = 0;
		throw std::runtime_error("Cannot map file. Error code: " + std::to_string(errno));
	}

	return bytes;
}

std::uint8_t * MMapDataProvider::Data() const
{
	if (m_dataPtr == nullptr || m_dataPtr == MAP_FAILED)
		return nullptr;
	return reinterpret_cast<uint8_t *>(m_dataPtr);
}

std::size_t MMapDataProvider::TotalSize() const
{
	return m_fileSize;
}

bool MMapDataProvider::Eof()
{
	return m_eof;
}
