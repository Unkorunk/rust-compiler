#pragma once

#include "InputStream.hpp"

class TokenizerHelper {
public:
    static bool IsWhitespace(char it) {
        return it == '\t' || it == '\n' || it == '\v' ||
            it == '\f' || it == '\r' || it == ' ';
    }

    static bool IsAlphanumeric(char it) {
        return it >= 'a' && it <= 'z' ||
            it >= 'A' && it <= 'Z' ||
            it >= '0' && it <= '9' ||
            it == '_';
    }

    static bool IsBinDigit(char it) {
        return it == '0' || it == '1';
    }

    static bool IsOctDigit(char it) {
        return it >= '0' && it <= '7';
    }

    static bool IsDecDigit(char it) {
        return it >= '0' && it <= '9';
    }

    static bool IsHexDigit(char it) {
        return IsDecDigit(it) || it >= 'a' && it <= 'f' ||
            it >= 'A' && it <= 'F';
    }

    static char BinToInt(char it) {
        if (it == '0') {
            return 0;
        } else if (it == '1') {
            return 1;
        }
        throw std::exception();
    }

    static char OctToInt(char it) {
        if (it >= '0' && it <= '7') {
            return (it - '0');
        }
        throw std::exception();
    }

    static char DecToInt(char it) {
        if (it >= '0' && it <= '9') {
            return (it - '0');
        }
        throw std::exception();
    }

    static char HexToInt(char it) {
        if (it >= 'a' && it <= 'f') {
            return 10 + (it - 'a');
        } else if (it >= 'A' && it <= 'F') {
            return 10 + (it - 'A');
        } else if (it >= '0' && it <= '9') {
            return (it - '0');
        }
        throw std::exception();
    }

    static bool TryGetEscape(InputStream *stream, char *result) {
        char c = stream->PeekChar(0);
        if (c == '\\') {
            c = stream->PeekChar(1);
            stream->SkipChar(2);

            if (c == '\'') {
                *result = '\'';
            } else if (c == '"') {
                *result = '\"';
            } else if (c == 'x') {
                c = stream->PeekChar(0);
                if (TokenizerHelper::IsOctDigit(c)) {
                    char symbol = TokenizerHelper::OctToInt(c);
                    c = stream->PeekChar(1);
                    stream->SkipChar(2);
                    if (TokenizerHelper::IsHexDigit(c)) {
                        symbol = (symbol * 16) + TokenizerHelper::HexToInt(c);
                        *result = symbol;
                    } else {
                        return false;
                    }
                } else {
                    stream->SkipChar(1);
                    return false;
                }
            } else if (c == 'n') {
                *result = '\n';
            } else if (c == 'r') {
                *result = '\r';
            } else if (c == 't') {
                *result = '\t';
            } else if (c == '\\') {
                *result = '\\';
            } else if (c == '0') {
                *result = '\0';
            } else {
                return false;
            }
        }

        return true;
    }

    static bool TryGetByteEscape(InputStream *stream, uint8_t *result) {
        char c = stream->PeekChar(0);
        if (c == '\\') {
            c = stream->PeekChar(1);
            stream->SkipChar(2);

            if (c == '\'') {
                *result = '\'';
            } else if (c == '"') {
                *result = '\"';
            } else if (c == 'x') {
                c = stream->PeekChar(0);
                if (TokenizerHelper::IsHexDigit(c)) {
                    uint8_t symbol = TokenizerHelper::HexToInt(c);
                    c = stream->PeekChar(1);
                    stream->SkipChar(2);
                    if (TokenizerHelper::IsHexDigit(c)) {
                        symbol = (symbol * 16) + TokenizerHelper::HexToInt(c);
                        *result = symbol;
                    } else {
                        return false;
                    }
                } else {
                    stream->SkipChar(1);
                    return false;
                }
            } else if (c == 'n') {
                *result = '\n';
            } else if (c == 'r') {
                *result = '\r';
            } else if (c == 't') {
                *result = '\t';
            } else if (c == '\\') {
                *result = '\\';
            } else if (c == '0') {
                *result = '\0';
            } else {
                return false;
            }
        }

        return true;
    }

private:

};