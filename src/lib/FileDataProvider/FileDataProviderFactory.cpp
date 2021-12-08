#include "FileDataProviderFactory.h"

#include <fcntl.h>
#include <unistd.h>

#include <boost/filesystem.hpp>

#include "FileDataProvider.h"

FileDataProviderFactory::FileDataProviderFactory(const std::string & file_path)
	: m_filePath(file_path)
	, m_fileDescriptor(-1)
//	, m_fileDescriptor(open(m_filePath.data(), O_RDONLY))
{
}

FileDataProviderFactory::~FileDataProviderFactory()
{
//	close(m_fileDescriptor);
}

std::unique_ptr<IDataProvider> FileDataProviderFactory::CreateDataProvider() const
{
	std::unique_ptr<IDataProvider> provider(std::make_unique<FileDataProvider>(m_filePath, m_fileDescriptor));
	provider->Initialize();
	return provider;
}
