#pragma once

#include <sstream>
#include <array>

#include "Token.hpp"

class Tokenizer {
public:
    using char_type = char32_t;
    using istream_type = std::basic_istream<char_type, std::char_traits<char_type>>;

    explicit Tokenizer(istream_type *stream);

    bool HasNext() const;
    Token Next();

private:
    istream_type *stream_;
    
    std::array<char_type, 2048> buf1_{}, buf2_;
    std::streamsize buf1_length_ = 0, buf2_length_ = 0;
    std::size_t buf1_position_ = 0;
    bool buf2_initialized_ = false;

    bool IsEOF() const;
    char_type PeekChar(size_t offset);
    void SkipChar(size_t offset);

    static bool IsWhitespace(char_type it);

    static bool IsAlphanumeric(char_type it);

    static bool IsBinDigit(char_type it);
    static bool IsOctDigit(char_type it);
    static bool IsDecDigit(char_type it);
    static bool IsHexDigit(char_type it);

    void SkipWhitespace();

    bool TryTokenizeKeyword(Token *token);
    Token TokenizeIdentifier();
    // character and string literals
    Token TokenizeCharacter();
    Token TokenizeString();
    Token TokenizeRawString();
    // byte and byte string literals
    Token TokenizeByte();
    Token TokenizeByteString();
    Token TokenizeRawByteString();
    // number literals
    Token TokenizeInteger();
    Token TokenizeFloat();
    // boolean literals
    Token TokenizeBoolean();
    // TODO lifetimes and loop labels

};
