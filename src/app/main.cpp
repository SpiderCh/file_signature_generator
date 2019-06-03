#include <thread>
#include <string>
#include <iostream>

#include <boost/program_options.hpp>

#include "SignatureCalculator.h"

#include "FileDataProvider.h"
#include "MD5HashCalculator.h"
#include "CRCHashCalculator.h"

namespace detail
{

struct KeyInfo
{
	const std::string key {};
	const std::string shorten_key {};
	const std::string clued_key {};

	explicit KeyInfo(const std::string & key)
		: key(key)
		, clued_key(key)
	{}

	KeyInfo(const std::string & key, const std::string & shorten_key)
		: key(key)
		, shorten_key(shorten_key)
		, clued_key(key + (shorten_key.empty() ? "" : "," + shorten_key))
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
	bool help_requested {false};

	std::string input_file;
	std::string output_file;
	HashAlgorithm algoritm {HashAlgorithm::md5};
	size_t block_size {1048576};
};

InputParameters ParseStartOptions(int argc, char** argv)
{
	boost::program_options::options_description desription;
	desription.add_options()
			(INPUT_FILE_KEY.clued_key.data(),  boost::program_options::value<std::string>(), "set path to file which must be hashed")
			(OUTPUT_FILE_KEY.clued_key.data(), boost::program_options::value<std::string>(), "set path for output file")
			(BLOCK_SIZE_KEY.clued_key.data(),  boost::program_options::value<size_t>(), "block size")
			(ALGORITM_TYPE.clued_key.data(),   boost::program_options::value<std::string>(), "use algoritm (md5 or crc)")
			(HELP_KEY.clued_key.data(), "show current help message")
	;

	boost::program_options::variables_map variablesMap;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desription), variablesMap);
	boost::program_options::notify(variablesMap);

	InputParameters parameters;
	parameters.help_requested = variablesMap.count(HELP_KEY.key);
	if (parameters.help_requested)
	{
		std::cout << desription << std::endl;
		return parameters;
	}

	if (variablesMap.count(INPUT_FILE_KEY.key))
		parameters.input_file = variablesMap[INPUT_FILE_KEY.key].as<std::string>();

	if (variablesMap.count(OUTPUT_FILE_KEY.key))
		parameters.output_file = variablesMap[OUTPUT_FILE_KEY.key].as<std::string>();

	if (variablesMap.count(BLOCK_SIZE_KEY.key))
		parameters.block_size = variablesMap[BLOCK_SIZE_KEY.key].as<size_t>();

	if (variablesMap.count(ALGORITM_TYPE.key))
		parameters.algoritm = variablesMap[ALGORITM_TYPE.key].as<std::string>() == "md5" ? InputParameters::HashAlgorithm::md5
																						 : InputParameters::HashAlgorithm::crc;

	return parameters;
}
} // namespace detail

int main(int argc, char** argv)
{
	const detail::InputParameters params = detail::ParseStartOptions(argc, argv);

	if (params.help_requested)
		return 0;

	if (params.input_file.empty() || params.output_file.empty() || params.block_size < 1)
	{
		std::cerr << "Some of input parameters invalid. Call " << argv[0] << " --help for information." << std::endl;
		return 1;
	}

	std::shared_ptr<IDataProvider> data_provider(std::make_shared<FileDataProvider>(params.input_file));
	const bool data_provider_initialized = data_provider->Initialize();
	assert(data_provider_initialized);

	std::shared_ptr<Hash::IHashCalculator> hash_calculator;
	if (params.algoritm == detail::InputParameters::HashAlgorithm::md5)
		hash_calculator = std::make_shared<Hash::MD5Hash>();
	else if (params.algoritm == detail::InputParameters::HashAlgorithm::crc)
		hash_calculator = std::make_shared<Hash::CRCHash>();

	Calculator::CalculatorManager c(data_provider, hash_calculator, params.block_size);
	c.Start();

	return 0;
}
