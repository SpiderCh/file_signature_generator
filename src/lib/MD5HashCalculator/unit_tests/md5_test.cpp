#include <string>

#include <boost/test/floating_point_comparison.hpp>
#include <boost/test/unit_test.hpp>

#include "MD5HashCalculator.h"

BOOST_AUTO_TEST_CASE(test_empty_string)
{
	const std::string str("");
	const std::vector<unsigned char> data(str.cbegin(), str.cend());

	Hash::MD5Hash hasher;
	const std::string result = hasher.CalculateHash(data);

	BOOST_CHECK_EQUAL(result, "d41d8cd98f00b204e9800998ecf8427e");
}

BOOST_AUTO_TEST_CASE(test_string_with_space)
{
	const std::string str(" ");
	const std::vector<unsigned char> data(str.cbegin(), str.cend());

	Hash::MD5Hash hasher;
	const std::string result = hasher.CalculateHash(data);

	BOOST_CHECK_EQUAL(result, "7215ee9c7d9dc229d2921a40e899ec5f");
}

BOOST_AUTO_TEST_CASE(test_md5_md5_string)
{
	const std::string str("md5");
	const std::vector<unsigned char> data(str.cbegin(), str.cend());

	Hash::MD5Hash hasher;
	const std::string result = hasher.CalculateHash(data);

	BOOST_CHECK_EQUAL(result, "1bc29b36f623ba82aaf6724fd3b16718");
}

BOOST_AUTO_TEST_CASE(test_md5_string_with_fox)
{
	const std::string string_data ("The quick brown fox jumps over the lazy dog");
	const std::vector<unsigned char> data (string_data.cbegin(), string_data.cend());

	Hash::MD5Hash hasher;
	const std::string result = hasher.CalculateHash(data);

	BOOST_CHECK_EQUAL(result, "9e107d9d372bb6826bd81d3542a419d6");
}

BOOST_AUTO_TEST_CASE(test_md5_string_with_alphabet)
{
	const std::string string_data ("abcdefghijklmnopqrstuvwxyz");
	const std::vector<unsigned char> data (string_data.cbegin(), string_data.cend());

	Hash::MD5Hash hasher;
	const std::string result = hasher.CalculateHash(data);

	BOOST_CHECK_EQUAL(result, "c3fcd3d76192e4007dfb496cca67e13b");
}

BOOST_AUTO_TEST_CASE(test_md5_string_with_alphabet_extended)
{
	const std::string string_data ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
	const std::vector<unsigned char> data (string_data.cbegin(), string_data.cend());

	Hash::MD5Hash hasher;
	const std::string result = hasher.CalculateHash(data);

	BOOST_CHECK_EQUAL(result, "d174ab98d277d9f5a5611c2c9f419d9f");
}

BOOST_AUTO_TEST_CASE(test_md5_long_string_with_numbers)
{
	const std::string string_data ("12345678901234567890123456789012345678901234567890123456789012345678901234567890");
	const std::vector<unsigned char> data (string_data.cbegin(), string_data.cend());

	Hash::MD5Hash hasher;
	const std::string result = hasher.CalculateHash(data);

	BOOST_CHECK_EQUAL(result, "57edf4a22be3c955ac49da2e2107b67a");
}

