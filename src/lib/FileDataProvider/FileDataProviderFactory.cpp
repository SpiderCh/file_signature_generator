#include "FileDataProviderFactory.h"

#include "FileDataProvider.h"

FileDataProviderFactory::FileDataProviderFactory(const std::string & file_path)
	: m_file_path(file_path)
{}

std::unique_ptr<IDataProvider> FileDataProviderFactory::CreateDataProvider() const
{
	std::unique_ptr<IDataProvider> provider(std::make_unique<FileDataProvider>(m_file_path));
	provider->Initialize();
	return provider;
}