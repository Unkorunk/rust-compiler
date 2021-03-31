cmake_minimum_required(VERSION 3.2)

project(rust-compiler)

set(CMAKE_CXX_STANDARD 17)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

add_executable(rust-compiler main.cpp
    Tokenizer.hpp Tokenizer.cpp
    Token.hpp
    Keyword.hpp KeywordManager.hpp
)
