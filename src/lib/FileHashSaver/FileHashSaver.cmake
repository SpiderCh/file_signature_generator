add_library(FileHashSaver SHARED "${CMAKE_CURRENT_LIST_DIR}/FileHashSaver.cpp"
									"${CMAKE_CURRENT_LIST_DIR}/FileHashSaver.h")
target_include_directories(FileHashSaver INTERFACE "${CMAKE_CURRENT_LIST_DIR}")

target_link_libraries(FileHashSaver InterfaceLib)
