cmake_minimum_required(VERSION 3.2)

project(rust-compiler)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_library(rust-compiler-tokenizer
    Tokenizer.hpp Tokenizer.cpp
    Token.hpp
    Keyword.hpp KeywordManager.hpp
    Punctuation.hpp
    InputStream.hpp
    TokenizerHelper.hpp
)

add_executable(rust-compiler main.cpp)
target_link_libraries(rust-compiler rust-compiler-tokenizer)

enable_testing()

add_executable(rust-compiler-tests tests.cpp)
target_link_libraries(rust-compiler-tests gtest_main rust-compiler-tokenizer)

add_custom_command(
        TARGET rust-compiler-tests PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/tests
                $<TARGET_FILE_DIR:rust-compiler-tests>/tests
)

include(GoogleTest)
gtest_discover_tests(rust-compiler-tests)