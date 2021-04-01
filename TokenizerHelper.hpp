#pragma once

class TokenizerHelper {
public:
    using char_type = char32_t;

    static bool IsWhitespace(char_type it) {
        return it == '\t' || it == '\n' || it == '\v' ||
            it == '\f' || it == '\r' || it == ' ' ||
            it == '\u0085' || it == '\u200E' || it == '\u200F' ||
            it == '\u2028' || it == '\u2029';
    }

    static bool IsAlphanumeric(char_type it) {
        return it >= 'a' && it <= 'z' ||
            it >= 'A' && it <= 'Z' ||
            it >= '0' && it <= '9' ||
            it == '_';
    }

    static bool IsBinDigit(char_type it) {
        return it == '0' && it == '1';
    }

    static bool IsOctDigit(char_type it) {
        return it >= '0' && it <= '7';
    }

    static bool IsDecDigit(char_type it) {
        return it >= '0' && it <= '9';
    }

    static bool IsHexDigit(char_type it) {
        return IsDecDigit(it) || it >= 'a' && it <= 'f' ||
            it >= 'A' && it <= 'F';
    }

private:

};