#ifndef FILE_DATA_PROVIDER_FACTORY_H
#define FILE_DATA_PROVIDER_FACTORY_H

#include <string>

#include "IDataProviderFactory.h"

#ifdef __APPLE__
#define DLL_EXPORT
#else
#define DLL_EXPORT __declspec(dllexport)
#endif

class DLL_EXPORT FileDataProviderFactory : public IDataProviderFactory
{
public:
	FileDataProviderFactory(const std::string & file_path);

	std::unique_ptr<IDataProvider> CreateDataProvider() const override;

private:
	const std::string m_file_path;
};

#undef DLL_EXPORT

#endif // FILE_DATA_PROVIDER_FACTORY_H