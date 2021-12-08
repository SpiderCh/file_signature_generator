#include "IFStreamDataProvider.h"

#include <boost/filesystem.hpp>

IFStreamDataProvider::IFStreamDataProvider(const std::string & filePath)
	: m_filePath(filePath)
	, m_fileSize(boost::filesystem::file_size(m_filePath))
{
	m_fileStream.open(m_filePath, std::ios_base::in | std::ifstream::binary);
	if (!m_fileStream.is_open())
		throw std::runtime_error("Cannot open file: " + m_filePath);
}

IFStreamDataProvider::~IFStreamDataProvider() = default;

size_t IFStreamDataProvider::Read(size_t from, size_t bytes)
{
	m_fileStream.seekg(from);
	// @note Trying read from stream. Setting eofbit if needed.
	m_fileStream.peek();

	if (m_fileStream.eof())
		return 0;

	if (bytes > m_fileSize - from)
		bytes = m_fileSize - from;

	m_data.clear();
	m_data.resize(bytes);
	char * begin = reinterpret_cast<char*>(m_data.data());
	m_fileStream.read(begin, bytes);
	return bytes;
}

const std::uint8_t * IFStreamDataProvider::Data() const
{
	if (m_data.empty())
		return nullptr;
	return m_data.data();
}

std::size_t IFStreamDataProvider::TotalSize() const
{
	return m_fileSize;
}

bool IFStreamDataProvider::Eof()
{
	return m_fileStream.eof();
}
