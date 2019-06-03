#ifndef FILE_HASH_SAVER_H
#define FILE_HASH_SAVER_H

#include <memory>
#include <string>

#include "IHashSaver.h"

#ifdef __APPLE__
	#define DLL_EXPORT
#else
	#define DLL_EXPORT __declspec(dllexport)
#endif

class DLL_EXPORT FileHashSaver : public IHashSaver
{

public:
	FileHashSaver(const std::string & file_path);
	~FileHashSaver();

	void Save(const std::string & hash) override;

private:
	struct Impl;
	std::unique_ptr<Impl> m_impl;
};

#undef DLL_EXPORT

#endif // FILE_HASH_SAVER_H
