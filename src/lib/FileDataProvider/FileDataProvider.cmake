add_library(FileDataProvider SHARED "${CMAKE_CURRENT_LIST_DIR}/FileDataProvider.cpp"
									"${CMAKE_CURRENT_LIST_DIR}/FileDataProvider.h")
target_include_directories(FileDataProvider INTERFACE "${CMAKE_CURRENT_LIST_DIR}")

target_link_libraries(FileDataProvider InterfaceLib)
