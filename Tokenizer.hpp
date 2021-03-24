#pragma once

#include <sstream>
#include <array>

#include "Token.hpp"

class Tokenizer {
public:
    explicit Tokenizer(std::istream *stream);

    bool HasNext() const;
    Token Next();

private:
    std::istream *stream_;
    
    std::array<std::istream::char_type, 4096> buf_{};
    std::streamsize buf_length_ = 0;
    std::size_t buf_position_ = 0;

    bool HasNextChar() const;
    std::istream::char_type NextChar();

};
