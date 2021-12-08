set(FileDataProviderSources "${CMAKE_CURRENT_LIST_DIR}/IFStreamDataProvider.cpp;${CMAKE_CURRENT_LIST_DIR}/IFStreamDataProvider.h")

# @note Windows do not support unix version of mmap
if (NOT WIN32)
	list(APPEND FileDataProviderSources "${CMAKE_CURRENT_LIST_DIR}/MMapDataProvider.h;${CMAKE_CURRENT_LIST_DIR}/MMapDataProvider.cpp")
endif()

add_library(FileDataProvider SHARED ${FileDataProviderSources})

target_include_directories(FileDataProvider INTERFACE "${CMAKE_CURRENT_LIST_DIR}")

target_link_libraries(FileDataProvider InterfaceLib Boost::filesystem)
