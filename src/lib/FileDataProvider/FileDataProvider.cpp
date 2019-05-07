#include <fstream>

#include "FileDataProvider.h"

struct FileDataProvider::Impl
{
	const std::string file_path;
	std::ifstream file_stream;

	Impl(const std::string & file_path)
		: file_path(file_path)
	{}
};

FileDataProvider::FileDataProvider(const std::string & file_path)
	: m_impl(std::make_unique<Impl>(file_path))
{}

FileDataProvider::~FileDataProvider() = default;

bool FileDataProvider::Initialize()
{
	if (m_impl->file_path.empty())
		return false;

	m_impl->file_stream.open(m_impl->file_path, std::ios_base::in);
	return m_impl->file_stream.is_open();
}

std::vector<char> FileDataProvider::Read(size_t bytes)
{
	if (m_impl->file_path.empty() || !m_impl->file_stream.is_open())
		throw std::runtime_error("No resource found");

	if (eof())
		throw std::runtime_error("No data available");

	std::vector<char> data;
	data.resize(bytes);
	m_impl->file_stream.read(data.data(), bytes);
	return data;
}

bool FileDataProvider::eof()
{
	return m_impl->file_stream.eof();
}
