#include <fstream>

#include "FileHashSaver.h"

FileHashSaver::FileHashSaver(const std::string & filePath)
	: m_filePath(filePath)
{
	m_fileStream.open(m_filePath);
	if (!m_fileStream.is_open())
		throw std::runtime_error("Cannot open file: " + m_filePath + "; for hash output.");
}

FileHashSaver::~FileHashSaver() = default;

void FileHashSaver::Save(const std::string & hash)
{
	m_fileStream << hash;
	m_fileStream.flush();
}
