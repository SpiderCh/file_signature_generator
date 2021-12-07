cmake_minimum_required(VERSION 3.20.3)

set(CMAKE_CXX_STANDARD 17)
project("signature_generator" VERSION 0.2)

set(CONANBUILD_INFO_FILE "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
set(CONANBUILD_INFO_MULTICONFIGURATION_FILE "${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake")
set(CONAN_OPTIONS "TARGETS;KEEP_RPATHS")

if (EXISTS "${CONANBUILD_INFO_MULTICONFIGURATION_FILE}")
	include(${CONANBUILD_INFO_MULTICONFIGURATION_FILE})
	set(CONAN_CMAKE_MULTI ON)
elseif (EXISTS "${CONANBUILD_INFO_FILE}")
	include(${CONANBUILD_INFO_FILE})
	list(APPEND CONAN_OPTIONS "NO_OUTPUT_DIRS")
	set(CONAN_CMAKE_MULTI OFF)
else()
	message(FATAL_ERROR "Cannot load any conanbuildinfo file.")
endif()

conan_basic_setup(${CONAN_OPTIONS})

find_package(Boost 1.76 COMPONENTS system program_options unit_test_framework REQUIRED)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

include("${SRC_DIR}/interfaces/Interface.cmake")
include("${SRC_DIR}/lib/FileHashSaver/FileHashSaver.cmake")
include("${SRC_DIR}/lib/FileDataProvider/FileDataProvider.cmake")
include("${SRC_DIR}/lib/MD5HashCalculator/MD5HashCalculator.cmake")
include("${SRC_DIR}/lib/CRCHashCalculator/CRCHashCalculator.cmake")

add_executable(${PROJECT_NAME}  ${SRC_DIR}/app/main.cpp
								${SRC_DIR}/app/SignatureCalculator.h
								${SRC_DIR}/app/SignatureCalculator.cpp)

target_link_libraries(${PROJECT_NAME} Boost::program_options
									  InterfaceLib
									  FileHashSaver
									  FileDataProvider
									  MD5HashCalculator
									  CRCHashCalculator)
