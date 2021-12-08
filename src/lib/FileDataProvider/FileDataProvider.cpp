#include <fstream>
#include <iostream>

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include <boost/filesystem.hpp>

#include "FileDataProvider.h"

FileDataProvider::FileDataProvider(const std::string & filePath, int fileDescriptor)
	: m_fileDescriptor(fileDescriptor)//(open(filePath.data(), O_RDONLY))
	, m_filePath(filePath)
{}

FileDataProvider::~FileDataProvider()
{
//	close(m_fileDescriptor);
}

bool FileDataProvider::Initialize()
{
	return true;
}

std::vector<std::uint8_t> FileDataProvider::Read(size_t from, size_t bytes)
{
	if (from >= boost::filesystem::file_size(m_filePath))
	{
		m_eof = true;
		return {};
	}

	if (from + bytes > boost::filesystem::file_size(m_filePath))
		bytes = boost::filesystem::file_size(m_filePath) - from;

	errno = 0;
	void * dataPtr = mmap(NULL, bytes, PROT_READ, MAP_SHARED, m_fileDescriptor, from);
//	std::cerr << errno << std::endl;

	std::vector<std::uint8_t> data;
	data.resize(bytes);
	char * begin = reinterpret_cast<char*>(data.data());
	memcpy(begin, dataPtr, bytes);
	munmap(dataPtr, bytes);

	return data;
}

bool FileDataProvider::eof()
{
	return m_eof;
}
