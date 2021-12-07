#include "MD5HashCalculator.h"

#include <cstdint>

#include <boost/algorithm/hex.hpp>
#include <boost/uuid/detail/md5.hpp>

namespace Hash
{
namespace detail
{

class CustomBackInserter
{
public:
	explicit CustomBackInserter(std::string & container)
		: m_container(&container)
	{}

	CustomBackInserter & operator=(const typename std::string::value_type & element)
	{
		m_container->push_back(std::tolower(element));
		return *this;
	}

	CustomBackInserter & operator*() { return *this; }
	CustomBackInserter & operator++() { return *this; }
	CustomBackInserter operator++(int) { return *this; }

private:
	std::string * m_container;
};

std::string ToString(const boost::uuids::detail::md5::digest_type & digest)
{
	const auto intDigest = reinterpret_cast<const unsigned int *>(&digest);
	std::string result;
	boost::algorithm::hex(intDigest, intDigest + (sizeof(boost::uuids::detail::md5::digest_type) / sizeof(int)), CustomBackInserter(result));
	return result;
}
} // namespace detail

std::string MD5Hash::CalculateHash(const std::vector<std::uint8_t> & data)
{
	/// @note This is private implementation of md5 hash algorithm from boost and better not to use it.
	/// There had been some issues (https://github.com/boostorg/uuid/issues/111) with backward compatibility.
	/// Personally, I would prefer using OpenSSL or Crypto++ libraries, but I have restrictions on available libraries.
	/// At last, we can always use our own implementation from commit e7259c52fef70fc465166ecf18339de5c0a2e850.
	boost::uuids::detail::md5 hash;
	boost::uuids::detail::md5::digest_type digest;

	hash.process_bytes(data.data(), data.size());
	hash.get_digest(digest);

	return detail::ToString(digest);
}

} // namespace Hash
