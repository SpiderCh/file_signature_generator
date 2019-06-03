#include <fstream>

#include "FileHashSaver.h"

struct FileHashSaver::Impl
{
	const std::string file_path;
	std::ofstream file_stream;

	Impl(const std::string & file_path)
		: file_path(file_path)
	{}
};

FileHashSaver::FileHashSaver(const std::string & file_path)
	: m_impl(std::make_unique<Impl>(file_path))
{
	m_impl->file_stream.open(file_path);
	if (!m_impl->file_stream.is_open())
		throw std::runtime_error("Cannot open file: " + file_path + "; for hash output.");
}

FileHashSaver::~FileHashSaver() = default;

void FileHashSaver::Save(const std::string &hash)
{
	m_impl->file_stream << hash;
	m_impl->file_stream.flush();
}
