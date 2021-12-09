#include <string>
#include <iostream>

#include <boost/program_options.hpp>

#include "SignatureCalculator.h"

#include "FileHashSaver.h"
#include "IFStreamDataProvider.h"
#include "MD5HashCalculator.h"
#include "CRCHashCalculator.h"

#if !defined(_WIN32) && !defined(_WIN64)
	#include "MMapDataProvider.h"
#endif

namespace detail
{

struct KeyInfo
{
	const std::string key {};
	const std::string shortenKey {};
	const std::string cluedKey {};

	explicit KeyInfo(const std::string & key)
		: key(key)
		, cluedKey(key)
	{}

	KeyInfo(const std::string & key, const std::string & shortenKey)
		: key(key)
		, shortenKey(shortenKey)
		, cluedKey(key + (shortenKey.empty() ? "" : "," + shortenKey))
	{}

};

const KeyInfo INPUT_FILE_KEY("input_file", "i");
const KeyInfo OUTPUT_FILE_KEY("output_file", "o");
const KeyInfo BLOCK_SIZE_KEY("block_size", "b");
const KeyInfo ALGORITM_TYPE("algorithm", "a");
const KeyInfo HELP_KEY("help", "h");

struct InputParameters
{
	enum class HashAlgorithm
	{
		md5,
		crc
	};
	bool helpRequested {false};

	std::string inputFile;
	std::string outputFile;
	HashAlgorithm algoritm {HashAlgorithm::md5};
	size_t blockSize {1048576};
};

InputParameters ParseStartOptions(int argc, char** argv)
{
	boost::program_options::options_description desription;
	desription.add_options()
			(INPUT_FILE_KEY.cluedKey.data(),  boost::program_options::value<std::string>(), "set path to file which must be hashed")
			(OUTPUT_FILE_KEY.cluedKey.data(), boost::program_options::value<std::string>(), "set path for output file")
			(BLOCK_SIZE_KEY.cluedKey.data(),  boost::program_options::value<size_t>(), "block size")
			(ALGORITM_TYPE.cluedKey.data(),   boost::program_options::value<std::string>(), "use algoritm (md5 or crc)")
			(HELP_KEY.cluedKey.data(), "show current help message")
	;

	boost::program_options::variables_map variablesMap;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desription), variablesMap);
	boost::program_options::notify(variablesMap);

	InputParameters parameters;
	parameters.helpRequested = variablesMap.count(HELP_KEY.key);
	if (parameters.helpRequested)
	{
		std::cout << desription << std::endl;
		return parameters;
	}

	if (variablesMap.count(INPUT_FILE_KEY.key))
		parameters.inputFile = variablesMap[INPUT_FILE_KEY.key].as<std::string>();

	if (variablesMap.count(OUTPUT_FILE_KEY.key))
		parameters.outputFile = variablesMap[OUTPUT_FILE_KEY.key].as<std::string>();

	if (variablesMap.count(BLOCK_SIZE_KEY.key))
		parameters.blockSize = variablesMap[BLOCK_SIZE_KEY.key].as<size_t>();

	if (variablesMap.count(ALGORITM_TYPE.key))
		parameters.algoritm = variablesMap[ALGORITM_TYPE.key].as<std::string>() == "md5" ? InputParameters::HashAlgorithm::md5
																						 : InputParameters::HashAlgorithm::crc;

	return parameters;
}

void AppendInvalidParameter(std::string & to, const std::string paramName)
{
	if (!to.empty())
		to.append(", ");

	to += paramName;
}

bool BlockSizeValid(size_t blockSize)
{
#if __x86_64__ || __arm64__ || __ppc64__ || _WIN64
	return true;
#else
	constexpr size_t FOUR_GB_IN_BYTES = 4294967296;
	return blockSize >= FOUR_GB_IN_BYTES;
#endif
}
} // namespace detail

int main(int argc, char** argv)
{
	const detail::InputParameters params = detail::ParseStartOptions(argc, argv);

	if (params.helpRequested)
		return 0;

	if (params.inputFile.empty() || params.outputFile.empty() || params.blockSize < 1)
	{
		std::string invalid_parameters;
		if (params.inputFile.empty())
			detail::AppendInvalidParameter(invalid_parameters, detail::INPUT_FILE_KEY.key);
		if (params.outputFile.empty())
			detail::AppendInvalidParameter(invalid_parameters, detail::OUTPUT_FILE_KEY.key);
		if (params.blockSize < 1 || detail::BlockSizeValid(params.blockSize))
			detail::AppendInvalidParameter(invalid_parameters, detail::BLOCK_SIZE_KEY.key);

		std::cerr << "Invalid parameters: " << invalid_parameters << "\nCall " << argv[0] << " --help for information." << std::endl;
		return 1;
	}

	try
	{
		std::shared_ptr<IHashSaver> hashSaver(std::make_shared<FileHashSaver>(params.outputFile));
		std::shared_ptr<Hash::IHashCalculator> hash_calculator;
		if (params.algoritm == detail::InputParameters::HashAlgorithm::md5)
			hash_calculator = std::make_shared<Hash::MD5Hash>();
		else if (params.algoritm == detail::InputParameters::HashAlgorithm::crc)
			hash_calculator = std::make_shared<Hash::CRCHash>();

		std::shared_ptr<IDataProvider> dataProvider;

#if !defined(_WIN32) && !defined(_WIN64)
		dataProvider = std::make_shared<MMapDataProvider>(params.inputFile);
#endif
		if (!dataProvider)
			dataProvider = std::make_shared<IFStreamDataProvider>(params.inputFile);

		Calculator::CalculatorManager c(dataProvider, hashSaver, hash_calculator, params.blockSize);
		c.Start();
	}
	catch(const std::exception & ex)
	{
		std::cerr << "Caught exception: " << ex.what() << std::endl;
		return 1;
	}

	return 0;
}
