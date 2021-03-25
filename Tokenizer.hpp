#pragma once

#include <sstream>
#include <array>
#include <fstream>

#include "Token.hpp"

class Tokenizer {
public:
    using char_type = char32_t;
    using stream_type = std::basic_ifstream<char_type, std::char_traits<char_type>>;

    explicit Tokenizer(stream_type *stream);

    bool HasNext() const;
    Token Next();

private:
    stream_type *stream_;

    bool IsEOF() const;
    char_type PeekChar(std::streamoff offset);
    void SkipChar(std::streamsize offset);

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
