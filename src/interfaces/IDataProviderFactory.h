#ifndef IDATA_PROVIDER_FACTORY_H
#define IDATA_PROVIDER_FACTORY_H

#include <memory>

class IDataProvider;

class IDataProviderFactory
{
public:
	virtual ~IDataProviderFactory() = default;

	virtual std::unique_ptr<IDataProvider> CreateDataProvider() const = 0;
};

#endif // IDATA_PROVIDER_FACTORY