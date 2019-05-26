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

	BOOST_CHECK_EQUAL(result, "7215ee9c7d9dcc29d2921a40e899ec5f");
}

//BOOST_AUTO_TEST_CASE(test_md5_string)
//{
//	const std::vector<char> data {'m', 'd', '5'};
//
//	Hash::MD5Hash hasher;
//	const std::string result = hasher.CalculateHash(data);
//
//	BOOST_CHECK_EQUAL(result, "1bc29b36f623ba82aaf6724fd3b16718");
//}
