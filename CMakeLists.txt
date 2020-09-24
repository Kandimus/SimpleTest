cmake_minimum_required(VERSION 3.5)

project(simpletest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(tests)
add_executable(${PROJECT_NAME} simpletest.h simpletest.gen.cpp example/main.cpp example/first.cpp)

find_package( PythonInterp 3.0 REQUIRED )

# Генерация исходного кода
add_custom_target(generating_cpp ALL
	COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/simpletest.py --output ${CMAKE_CURRENT_SOURCE_DIR}/simpletest.gen.cpp --dir ${CMAKE_CURRENT_SOURCE_DIR}/example --files first.cpp
	BYPRODUCTS ${CMAKE_SOURCE_DIR}/simpletest.gen.cpp & sleep 1s
	COMMENT "Generating simple test file")
add_dependencies(${PROJECT_NAME} generating_cpp)

