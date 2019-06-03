#ifndef IHASH_SAVER_H
#define IHASH_SAVER_H

#include <string>

class IHashSaver
{
public:
	virtual ~IHashSaver() = default;

	virtual void Save(const std::string & hash) = 0;
};

#endif // IHASH_SERVER_H