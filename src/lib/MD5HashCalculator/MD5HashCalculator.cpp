#include "MD5HashCalculator.h"

#include <array>
#include <cstdint>

#include <iostream>
#include <iomanip>
#include <cstdint>

namespace Hash
{
namespace detail
{
constexpr char * hex_chars = "0123456789abcdef";

constexpr std::array<std::uint32_t, 64> s {
	7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
	5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
	4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
	6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
};

constexpr std::array<std::uint32_t, 64> K {
		0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
		0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
		0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
		0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
		0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
		0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
		0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
		0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
		0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
		0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
		0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
		0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
		0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
		0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
		0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
		0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

class MD5
{
	std::uint32_t a0 = 0x67452301;
	std::uint32_t b0 = 0xefcdab89;
	std::uint32_t c0 = 0x98badcfe;
	std::uint32_t d0 = 0x10325476;

	std::array<std::uint32_t, 16> array;

public:
	void data_to_array(const int begin, const std::vector<unsigned char> & data)
	{
		for (int i = 0; i < 64;)
		{
			const int pos = i / 4;
			array[pos]  = data[begin + i++];
			array[pos] |= data[begin + i++] << 8;
			array[pos] |= data[begin + i++] << 16;
			array[pos] |= data[begin + i++] << 24;
		}

	}

	void claculate_hash()
	{
		std::uint32_t A = a0;
		std::uint32_t B = b0;
		std::uint32_t C = c0;
		std::uint32_t D = d0;

		for (int i = 0; i < 64; ++i)
		{
			int g = 0;
			std::uint32_t F = 0;
			if (i <= 15) {
				F = (B & C) | ((~B) & D);
				g = i;
			} else if (i <= 31) {
				F = (D & B) | ((~D) & C);
				g = (5 * i + 1) % 16;
			} else if (i <= 47) {
				F = B ^ C ^ D;
				g = (3 * i + 5) % 16;
			} else {
				F = C ^ (B | (~D));
				g = (7 * i) % 16;
			}
			F = F + A + detail::K[i] + array[g];
			A = D;
			D = C;
			C = B;
			B += ((F << s[i]) | (F >> (32 - s[i])));
		}

		a0 += A;
		b0 += B;
		c0 += C;
		d0 += D;
	}

	std::string to_string()
	{
		std::string result;
		const std::array<std::uint32_t, 4> data {a0, b0, c0, d0};

		for (const std::uint32_t & value : data)
		{
			std::cout << std::setfill('0') << std::setw(8) << std::hex << value << '\n';
			for (auto i = 0u; i < 32; i += 4)
				result.push_back(hex_chars[(value >> (i ^ 4)) & 0xf]);
		}

		return result;
	}
};
} // namespace detail

std::string MD5Hash::CalculateHash(std::vector<unsigned char> data)
{
	const uint64_t delimiter = 18446744073709551615;
	const uint64_t original_data_length_bits = data.size() * 8 ;

	// @note Appending 1 bit to original data.
	data.push_back(128);

	while ((data.size() * 8) % 512 != 448)
		data.push_back(0);

	for (int i = 0; i < 8; ++i)
		data.push_back(0);

	const uint64_t size_to_write = original_data_length_bits % delimiter;

	data[data.size() - 9] = size_to_write & 0x7F80000000000000;
	data[data.size() - 7] = size_to_write & 0x7F800000000000;
	data[data.size() - 6] = size_to_write & 0x7F8000000000;
	data[data.size() - 5] = size_to_write & 0x7F80000000;
	data[data.size() - 4] = size_to_write & 0x7F800000;
	data[data.size() - 3] = size_to_write & 0x7F8000;
	data[data.size() - 2] = size_to_write & 0x7F80;
	data[data.size() - 1] = size_to_write & 0x7F;

//	assert((data.size() * 8) % 512 == 448);

	detail::MD5 md5;
	const int last_block_count = (data.size() * 8) / 512 + 1;
	for (int i = 0; i < last_block_count; i += 64)
	{
		md5.data_to_array(i, data);
		md5.claculate_hash();
	}

	return md5.to_string();
}

} // namespace Hash

