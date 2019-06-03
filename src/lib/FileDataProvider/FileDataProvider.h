#ifndef FILE_DATA_PROVIDER_H
#define FILE_DATA_PROVIDER_H

#include <memory>
#include <string>

#include "IDataProvider.h"

class FileDataProvider : public IDataProvider
{

public:
	FileDataProvider(const std::string & file_path);
	~FileDataProvider();

	bool Initialize() override;

	std::vector<std::uint8_t> Read(size_t from, size_t bytes) override;
	bool eof() override;

private:
	struct Impl;
	std::unique_ptr<Impl> m_impl;
};

#endif
