#include "Tokenizer.hpp"

Tokenizer::Tokenizer(std::ifstream *stream, TargetType target_type) : stream_(stream), target_type_(target_type) {}

bool Tokenizer::HasNext() const {
    return !stream_.IsEOF();
}

Token Tokenizer::Next() {
    SkipWhitespace();

    if (stream_.IsEOF()) {
        return MakeToken(Token::Type::kEndOfFile);
    }

    stream_.AssignStart();

    char c0 = stream_.PeekChar(0);
    char c1 = stream_.PeekChar(1);
    char c2 = stream_.PeekChar(2);

    if (TokenizerHelper::IsDecDigit(c0)) {
        return TokenizeNumber();
    } else if (
        c0 == '_' && TokenizerHelper::IsAlphanumeric(c1) || c0 >= 'a' && c0 <= 'z' && c0 != 'r' && c0 != 'b' ||
        c0 >= 'A' && c0 <= 'Z' ||
        c0 == 'r' && (c1 != '"' && (c1 != '#' || c2 != '"' && c2 != '#')) || //! raw string literals
        c0 == 'b' && c1 != '\'' && c1 != '"' && (c1 != 'r' || c2 != '"' && c2 != '#') || //! byte and byte string literals
        c0 == '\'' && ((c1 == '_' || c1 >= 'a' && c1 <= 'z' || c1 >= 'A' && c1 <= 'Z') && c2 != '\'')) { //! lifetimes and loop labels
        return TokenizeIdentifierOrKeyword();
    }

    for (const Punctuation &punctuation : punctuation_) {
        Token token;
        if (punctuation.TryTokenize(&stream_, &token)) {
            return token;
        }
    }

    stream_.SkipChar(1);
    return MakeError("unexpected symbol");
}

void Tokenizer::SkipWhitespace() {
    while (!stream_.IsEOF() && TokenizerHelper::IsWhitespace(stream_.PeekChar(0))) {
        stream_.SkipChar(1);
    }
}

Token Tokenizer::SkipLineComment() {
    while (!stream_.IsEOF() && stream_.PeekChar(0) != '\n') {
        stream_.SkipChar(1);
    }
    return Next();
}

Token Tokenizer::SkipMultilineComment() {
    bool is_start_found = false;

    int comment_balance = 1;
    while (!stream_.IsEOF() && comment_balance != 0) {
        char c = stream_.PeekChar(0);
        if (c == '/' && stream_.PeekChar(1) == '*') {
            comment_balance++;
        } else if (c == '*') {
            is_start_found = true;
        } else if (c == '/' && is_start_found) {
            if (comment_balance == 0) {
                stream_.SkipChar(1);
                return MakeError("extra terminated block comment");
            }
            comment_balance--;
        } else {
            is_start_found = false;
        }
        stream_.SkipChar(1);
    }

    return Next();
}

Token Tokenizer::TokenizeIdentifierOrKeyword() {
    std::string identifier_buf;

    bool is_raw_identifier = false;
    bool is_quote_found = false;

    char c = stream_.PeekChar(0);
    if (c == 'r' && stream_.PeekChar(1) == '#') {
        is_raw_identifier = true;
        stream_.SkipChar(2);
        c = stream_.PeekChar(0);
    }

    if (c == '\'') {
        identifier_buf += c;

        is_quote_found = true;
        stream_.SkipChar(1);
        c = stream_.PeekChar(0);
    }

    if (is_raw_identifier && is_quote_found) {
        return MakeError("found invalid character; only `#` is allowed in raw string delimitation: '");
    }

    identifier_buf += c;

    if (c == '_') {
        if (is_quote_found) {
            return MakeToken(TokenValue(std::string("_")), Token::Type::kLifetimeOrLabel);
        }

        stream_.SkipChar(1);
        c = stream_.PeekChar(0);
        if (!TokenizerHelper::IsAlphanumeric(c)) {
            return MakeError("expected alphanumeric symbol");
        }
    } else if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z') {
        stream_.SkipChar(1);
        c = stream_.PeekChar(0);
    } else {
        return MakeError("expected alphanumeric or `_` symbol");
    }

    while (TokenizerHelper::IsAlphanumeric(c)) {
        identifier_buf += c;
        stream_.SkipChar(1);
        c = stream_.PeekChar(0);
    }

    if (is_raw_identifier) {
        if (identifier_buf != "crate" && identifier_buf != "self" && identifier_buf != "super" &&
            identifier_buf != "Self") {
            return MakeIdentifier(identifier_buf);
        }
    } else if (!KeywordManager::GetInstance().IsStrictOrReservedKeyword(identifier_buf)) {
        return MakeIdentifier(identifier_buf);
    } else if (const Keyword *keyword = KeywordManager::GetInstance().Find(identifier_buf)) {
        if (keyword->GetTokenType() == Token::Type::kTrue) {
            return MakeLiteral(true);
        } else if (keyword->GetTokenType() == Token::Type::kFalse) {
            return MakeLiteral(false);
        }

        return MakeToken(keyword->GetTokenType());
    }

    if (is_quote_found) {
        identifier_buf.erase(identifier_buf.begin());
        return MakeToken(TokenValue(identifier_buf), Token::Type::kLifetimeOrLabel);
    }

    return MakeError("invalid sequence of characters");
}

Token Tokenizer::TokenizeCharacter() {
    char c = stream_.PeekChar(0);
    if (c == '\'' || c == '\n' || c == '\r' || c == '\t') {
        stream_.SkipChar(1);
        return MakeError("character constant must be escaped");
    }

    if (c == '\\') {
        if (!TokenizerHelper::TryGetEscape(&stream_, &c)) {
            return MakeError("unknown character escape");
        }
    } else {
        stream_.SkipChar(1);
    }

    if (stream_.PeekChar(0) != '\'') {
        return MakeError("unterminated character literal");
    }

    stream_.SkipChar(1);

    return MakeLiteral(c);
}

Token Tokenizer::TokenizeString() {
    std::string string_buf;

    while (true) {
        char c = stream_.PeekChar(0);
        if (stream_.IsEOF()) {
            return MakeError("unexpected end-of-file");
        }

        if (c == '\"') {
            stream_.SkipChar(1);
            break;
        }

        // ~IsolatedCR
        if (c == '\r' && stream_.PeekChar(1) != '\n') {
            return MakeError("invalid sequence of characters");
        }

        if (c == '\n') {
            return MakeError("unterminated string literal");
        }

        if (c == '\\') {
            if (stream_.PeekChar(1) == '\n') {
                stream_.SkipChar(2);
                SkipWhitespace();
                continue;
            } else if (!TokenizerHelper::TryGetEscape(&stream_, &c)) {
                return MakeError("unknown character escape");
            }
        } else {
            stream_.SkipChar(1);
        }

        string_buf += c;
    }

    return MakeLiteral(string_buf);
}

Token Tokenizer::TokenizeRawString() {
    std::string raw_string_buf;

    int hash_require = 0;
    while (stream_.PeekChar(0) == '#') {
        stream_.SkipChar(1);
        hash_require++;
    }

    char c = stream_.PeekChar(0);
    if (c != '\"') {
        return MakeError("expected `\"`");
    }

    stream_.SkipChar(1);

    bool is_double_quote_found = false;
    int hash_count = 0;

    c = stream_.PeekChar(0);
    while (!is_double_quote_found || hash_count != hash_require) {
        if (stream_.IsEOF()) {
            return MakeError("unexpected end-of-file");
        }

        if (c == '"') {
            is_double_quote_found = true;
            hash_count = 0;
        } else if (is_double_quote_found && c == '#') {
            hash_count++;
        } else {
            is_double_quote_found = false;
            hash_count = 0;
        }

        raw_string_buf += c;

        stream_.SkipChar(1);
        c = stream_.PeekChar(0);
    }

    raw_string_buf.resize(raw_string_buf.size() - hash_require - 1);
    return MakeLiteral(raw_string_buf);
}

Token Tokenizer::TokenizeByte() {
    char c = stream_.PeekChar(0);
    if (c == '\'' || c == '\n' || c == '\r' || c == '\t') {
        stream_.SkipChar(1);
        return MakeError("character constant must be escaped");
    }

    uint8_t result;

    if (c == '\\') {
        if (!TokenizerHelper::TryGetByteEscape(&stream_, &result)) {
            return MakeError("unknown character escape");
        }
    } else if (c >= 0x00 && c <= 0x7f) {
        stream_.SkipChar(1);
        result = c;
    } else {
        stream_.SkipChar(1);
        return MakeError("invalid sequence of characters");
    }

    if (stream_.PeekChar(0) != '\'') {
        return MakeError("unterminated byte literal");
    }

    stream_.SkipChar(1);

    return MakeLiteral(result);
}

Token Tokenizer::TokenizeByteString() {
    std::vector<uint8_t> byte_string_buf;

    while (true) {
        char c = stream_.PeekChar(0);
        if (stream_.IsEOF()) {
            return MakeError("unexpected end-of-file");
        }

        if (c == '\"') {
            stream_.SkipChar(1);
            break;
        }

        // ~IsolatedCR
        if (c == '\r' && stream_.PeekChar(1) != '\n') {
            return MakeError("invalid sequence of characters");
        }

        if (c == '\n') {
            return MakeError("unterminated byte string literal");
        }

        uint8_t result;

        if (c == '\\') {
            if (stream_.PeekChar(1) == '\n') {
                stream_.SkipChar(2);
                SkipWhitespace();
                continue;
            } else if (!TokenizerHelper::TryGetByteEscape(&stream_, &result)) {
                return MakeError("unknown character escape");
            }
        } else if (c >= 0x00 && c <= 0x7f) {
            stream_.SkipChar(1);
            result = c;
        } else {
            stream_.SkipChar(1);
            return MakeError("invalid sequence of characters");
        }

        byte_string_buf.push_back(result);
    }

    return MakeLiteral(byte_string_buf);
}

Token Tokenizer::TokenizeRawByteString() {
    std::vector<uint8_t> raw_byte_string_buf;

    int hash_require = 0;
    while (stream_.PeekChar(0) == '#') {
        stream_.SkipChar(1);
        hash_require++;
    }

    char c = stream_.PeekChar(0);
    if (c != '\"') {
        return MakeError("expected `\"`");
    }

    stream_.SkipChar(1);

    bool is_double_quote_found = false;
    int hash_count = 0;

    c = stream_.PeekChar(0);
    while (!is_double_quote_found || hash_count != hash_require) {
        if (stream_.IsEOF()) {
            return MakeError("unexpected end-of-file");
        }

        if (c == '"') {
            is_double_quote_found = true;
            hash_count = 0;
        } else if (is_double_quote_found && c == '#') {
            hash_count++;
        } else {
            is_double_quote_found = false;
            hash_count = 0;
        }

        if (c >= 0x00 && c <= 0x7f) {
            raw_byte_string_buf.push_back(c);
        } else {
            return MakeError("invalid sequence of characters");
        }

        stream_.SkipChar(1);
        c = stream_.PeekChar(0);
    }

    raw_byte_string_buf.resize(raw_byte_string_buf.size() - hash_require - 1);
    return MakeLiteral(raw_byte_string_buf);
}

Token Tokenizer::TokenizeNumber() {
    bool is_digit_found = false;

    char c = stream_.PeekChar(0);
    if (!TokenizerHelper::IsDecDigit(c)) {
        throw std::exception();
    }

    int system = (c != '0' ? 10 : -1);
    if (system == -1) {
        stream_.SkipChar(1);
        c = stream_.PeekChar(0);

        if (c == 'b') {
            system = 2;
            stream_.SkipChar(1);
            c = stream_.PeekChar(0);
        } else if (c == 'o') {
            system = 8;
            stream_.SkipChar(1);
            c = stream_.PeekChar(0);
        } else if (c == 'x') {
            system = 16;
            stream_.SkipChar(1);
            c = stream_.PeekChar(0);
        } else {
            system = 10;
            is_digit_found = true;
        }
    }

    std::vector<int8_t> digits;
    digits.push_back(0);

    do {
        if (system == 2 && TokenizerHelper::IsBinDigit(c) || system == 8 && TokenizerHelper::IsOctDigit(c) ||
            system == 10 && TokenizerHelper::IsDecDigit(c))
        {
            digits.push_back(c - '0');
            is_digit_found = true;
        } else if (system == 16 && TokenizerHelper::IsHexDigit(c)) {
            digits.push_back(TokenizerHelper::HexToInt(c));

            is_digit_found = true;
        } else if (c != '_') {
            break;
        }

        stream_.SkipChar(1);
        c = stream_.PeekChar(0);
    } while (!stream_.IsEOF());

    if (!is_digit_found) {
        return MakeError("no valid digits found for number");
    }

    if (system == 10 && !stream_.IsEOF()) {
        bool is_dot_found = false;
        bool is_digit_after_dot_found = false;
        bool is_exponent_found = false;

        std::string float_str;
        for (int8_t digit : digits) {
            float_str += '0' + digit;
        }

        if (c == '.') {
            is_dot_found = true;
            float_str += '.';

            stream_.SkipChar(1);
            c = stream_.PeekChar(0);
            if (c == '_') {
                return MakeError("unexpected `_`");
            }
            while (TokenizerHelper::IsDecDigit(c) || c == '_') {
                if (c != '_') {
                    is_digit_after_dot_found = true;
                    float_str += c;
                }
                stream_.SkipChar(1);
                c = stream_.PeekChar(0);
            }
        }

        if (c == 'e' || c == 'E') {
            bool is_digit_after_exponent_found = false;

            is_exponent_found = true;
            float_str += 'e';

            stream_.SkipChar(1);
            c = stream_.PeekChar(0);
            if (c == '+' || c == '-') {
                float_str += c;
                stream_.SkipChar(1);
                c = stream_.PeekChar(0);
            }

            while (TokenizerHelper::IsDecDigit(c) || c == '_') {
                if (c != '_') {
                    is_digit_after_exponent_found = true;
                    float_str += c;
                }
                stream_.SkipChar(1);
                c = stream_.PeekChar(0);
            }

            if (!is_digit_after_exponent_found) {
                return MakeError("expected at least one digit in exponent");
            }
        }

        if (c == 'f') {
            if (is_dot_found && !is_digit_after_dot_found) {
                return MakeError("expected at least one digit after dot");
            }

            if (stream_.CheckSeq(1, {'3', '2'})) {
                stream_.SkipChar(3);
                return MakeLiteral(TokenValue(std::stof(float_str)));
            } else if (stream_.CheckSeq(1, {'6', '4'})) {
                stream_.SkipChar(3);
                return MakeLiteral(TokenValue(std::stod(float_str)));
            }
        }

        if (is_dot_found || is_exponent_found) {
            if (is_exponent_found && is_dot_found && !is_digit_after_dot_found) {
                return MakeError("expected at least one digit after dot");
            }
            return MakeLiteral(TokenValue(std::stod(float_str)));
        }
    }

    TokenValue token_value;
    if (!TryParse<uint8_t, uint16_t, uint32_t, uint64_t>(digits, &token_value, system)) {
        return MakeError("literal out of range");
    }

#define TP(type)                                   \
    type result;                                   \
    if (TryParse<type>(digits, &result, system)) { \
        token_value = result;                      \
    } else {                                       \
        return MakeError("literal out of range");  \
    }

#define TP_BRANCH(utype, itype) \
    if (c == 'u') {             \
        TP(utype)               \
    } else if (c == 'i') {      \
        TP(itype)               \
    }

    if (!stream_.IsEOF() && (c == 'i' || c == 'u')) {
        if (stream_.CheckSeq(1, {'8'})) {
            stream_.SkipChar(2);
            TP_BRANCH(uint8_t, int8_t)
        } else if (stream_.CheckSeq(1, {'1', '6'})) {
            stream_.SkipChar(3);
            TP_BRANCH(uint16_t, int16_t)
        } else if (stream_.CheckSeq(1, {'3', '2'})) {
            stream_.SkipChar(3);
            TP_BRANCH(uint32_t, int32_t)
        } else if (stream_.CheckSeq(1, {'6', '4'})) {
            stream_.SkipChar(3);
            TP_BRANCH(uint64_t, int64_t)
        } else if (stream_.CheckSeq(1, {'s', 'i', 'z', 'e'})) {
            stream_.SkipChar(5);
            if (target_type_ == TargetType::kX32) {
                TP_BRANCH(uint32_t, int32_t)
            } else if (target_type_ == TargetType::kX64) {
                TP_BRANCH(uint64_t, int64_t)
            } else {
                throw std::exception();  // not implemented
            }
        }
    }

    if (TokenizerHelper::IsAlphanumeric(stream_.PeekChar(0))) {
        return MakeError("unexpected symbol");
    }

    return MakeLiteral(token_value);
}
