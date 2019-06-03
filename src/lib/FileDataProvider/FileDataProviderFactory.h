#ifndef FILE_DATA_PROVIDER_FACTORY_H
#define FILE_DATA_PROVIDER_FACTORY_H

#include <string>

#include "IDataProviderFactory.h"

class FileDataProviderFactory : public IDataProviderFactory
{
public:
	FileDataProviderFactory(const std::string & file_path);

	std::unique_ptr<IDataProvider> CreateDataProvider() const override;

private:
	const std::string m_file_path;
};

#endif // FILE_DATA_PROVIDER_FACTORY_H