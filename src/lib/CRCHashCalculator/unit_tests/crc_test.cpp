#include <string>

#include <boost/test/floating_point_comparison.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test.hpp>

#include "CRCHashCalculator.h"

BOOST_AUTO_TEST_CASE(crc_test_with_empty_string)
{
	const std::string str("");
	const std::vector<std::uint8_t> data(str.cbegin(), str.cend());

	Hash::CRCHash hasher;
	const std::string result = hasher.CalculateHash(data);

	BOOST_CHECK_EQUAL(result, "00000000");
}

BOOST_AUTO_TEST_CASE(crc_test_string_with_space)
{
	const std::string str(" ");
	const std::vector<std::uint8_t> data(str.cbegin(), str.cend());

	Hash::CRCHash hasher;
	const std::string result = hasher.CalculateHash(data);

	BOOST_CHECK_EQUAL(result, "e96ccf45");
}

BOOST_AUTO_TEST_CASE(crc_test_crc_string)
{
	const std::string str("crc");
	const std::vector<std::uint8_t> data(str.cbegin(), str.cend());

	Hash::CRCHash hasher;
	const std::string result = hasher.CalculateHash(data);

	BOOST_CHECK_EQUAL(result, "7c6287fd");
}


BOOST_AUTO_TEST_CASE(crc_test_with_default_string)
{
	const std::string str("The quick brown fox jumps over the lazy dog");
	const std::vector<std::uint8_t> data(str.cbegin(), str.cend());

	Hash::CRCHash hasher;
	const std::string result = hasher.CalculateHash(data);

	BOOST_CHECK_EQUAL(result, "414fa339");
}


