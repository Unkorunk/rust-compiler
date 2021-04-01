#include "Tokenizer.hpp"

Tokenizer::Tokenizer(stream_type *stream) : stream_(stream) {}

bool Tokenizer::HasNext() const {
    return !stream_.IsEOF();
}

Token Tokenizer::Next() {
    SkipWhitespace();

    if (stream_.IsEOF()) {
        return MakeToken(Token::Type::kEndOfFile);
    }

    stream_.AssignStart();

    char_type c = stream_.PeekChar(0);
    char_type c1 = stream_.PeekChar(1);
    char_type c2 = stream_.PeekChar(2);
    if (TokenizerHelper::IsDecDigit(c)) {
        return TokenizeNumber();
    } else if (c == '_' && TokenizerHelper::IsAlphanumeric(stream_.PeekChar(1)) ||
        c >= 'a' && c <= 'z' && c != 'r' && c != 'b' ||
        c >= 'A' && c <= 'Z' ||
        c == 'r' && (c1 != '"' && (c1 != '#' || c2 != '"' && c2 != '#')) || //! raw string literals
        c == 'b' && c1 != '\'' && c1 != '"' && (c1 != 'r' || c2 != '"' && c2 != '#')) { //! byte and byte string literals
        return TokenizeIdentifierOrKeyword();
    }

    for (const Punctuation& punctuation : punctuation_) {
        Token token;
        if (punctuation.TryTokenize(&stream_, &token)) {
            return token;
        }
    }

    return MakeError("TODO");
}

void Tokenizer::SkipWhitespace() {
    while (!stream_.IsEOF() && TokenizerHelper::IsWhitespace(stream_.PeekChar(0))) {
        stream_.SkipChar(1);
    }
}

void Tokenizer::SkipLineComment() {
    while(!stream_.IsEOF() && stream_.PeekChar(0) != '\n') {
        stream_.SkipChar(1);
    }
}

void Tokenizer::SkipMultilineComment() {
    bool is_start_found = false;

    std::stack<bool> comment_stack;
    comment_stack.push(1);
    while (!stream_.IsEOF() && !comment_stack.empty()) {
        char_type c = stream_.PeekChar(0);
        if (c == '/' && stream_.PeekChar(1) == '*') {
            comment_stack.push(1);
        } else if (c == '*') {
            is_start_found = true;
        } else if (c == '/' && is_start_found) {
            if (comment_stack.empty() || comment_stack.top() != 1) {
                stream_.SkipChar(1);
                throw std::exception(); // TODO PushError("blabla")
            }
            comment_stack.pop();
        } else {
            is_start_found = false;
        }
        stream_.SkipChar(1);
    }
}

Token Tokenizer::TokenizeIdentifierOrKeyword() {
    bool is_raw_identifier = false;

    char_type c = stream_.PeekChar(0);
    if (c == 'r' && stream_.PeekChar(1) == '#') {
        is_raw_identifier = true;
        stream_.SkipChar(2);
        c = stream_.PeekChar(0);
    }

    std::string identifier_buf;
    identifier_buf += c;

    if (c == '_') {
        stream_.SkipChar(1);
        c = stream_.PeekChar(0);
        if (!TokenizerHelper::IsAlphanumeric(c)) {
            return MakeError("TODO");
        }
    } else if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z') {
        stream_.SkipChar(1);
        c = stream_.PeekChar(0);
    } else {
        return MakeError("TODO");
    }

    while (TokenizerHelper::IsAlphanumeric(c)) {
        identifier_buf += c;
        stream_.SkipChar(1);
        c = stream_.PeekChar(0);
    }

    if (is_raw_identifier) {
        if (identifier_buf != "crate" && identifier_buf != "self" && identifier_buf != "super" && identifier_buf != "Self") {
            return MakeIdentifier(identifier_buf);
        }
    } else if (!KeywordManager::GetInstance().IsStrictOrReservedKeyword(identifier_buf)) {
        return MakeIdentifier(identifier_buf);
    } else if (const Keyword* keyword = KeywordManager::GetInstance().Find(identifier_buf)) {
        if (keyword->GetTokenType() == Token::Type::kTrue) {
            return MakeLiteral(true);
        } else if (keyword->GetTokenType() == Token::Type::kFalse) {
            return MakeLiteral(false);
        }

        return MakeToken(keyword->GetTokenType());
    }

    return MakeError("TODO");
}

Token Tokenizer::TokenizeCharacter() {
    // TODO
    return MakeToken(Token::Type::kLiteral);
}

Token Tokenizer::TokenizeString() {
    // TODO
    return MakeToken(Token::Type::kLiteral);
}

Token Tokenizer::TokenizeRawString() {
    // TODO
    return MakeToken(Token::Type::kLiteral);
}

Token Tokenizer::TokenizeByte() {
    // TODO
    return MakeToken(Token::Type::kLiteral);
}

Token Tokenizer::TokenizeByteString() {
    // TODO
    return MakeToken(Token::Type::kLiteral);
}

Token Tokenizer::TokenizeRawByteString() {
    // TODO
    return MakeToken(Token::Type::kLiteral);
}

Token Tokenizer::TokenizeNumber() {
    bool is_digit_found = false;

    char_type c = stream_.PeekChar(0);
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

    do {
        if (system == 2 && TokenizerHelper::IsBinDigit(c) || system == 8 && TokenizerHelper::IsOctDigit(c) ||
            system == 10 && TokenizerHelper::IsDecDigit(c)) {
            digits.push_back(c - '0');
            is_digit_found = true;
        } else if (system == 16 && TokenizerHelper::IsHexDigit(c)) {
            if (c >= '0' && c <= '9') {
                digits.push_back(c - '0');
            } else if (c >= 'a' && c <= 'f') {
                digits.push_back(10 + (c - 'a'));
            } else {
                digits.push_back(10 + (c - 'A'));
            }

            is_digit_found = true;
        } else if (c != '_') {
            break;
        }

        stream_.SkipChar(1);
        c = stream_.PeekChar(0);
    } while (!stream_.IsEOF());

    if (!is_digit_found) {
        return MakeError("TODO");
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
            while (TokenizerHelper::IsDecDigit(c)) {
                is_digit_after_dot_found = true;
                float_str += c;
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
            }

            stream_.SkipChar(1);
            c = stream_.PeekChar(0);
            while(TokenizerHelper::IsDecDigit(c) || c == '_') {
                if (c != '_') {
                    is_digit_after_exponent_found = true;
                    float_str += c;
                }
                stream_.SkipChar(1);
                c = stream_.PeekChar(0);
            }

            if (!is_digit_after_exponent_found) {
                return MakeError("TODO");
            }
        }

        if (c == 'f') {
            if (is_dot_found && !is_digit_after_dot_found) {
                return MakeError("TODO");
            }

            if (stream_.CheckSeq(1, { '3', '2' })) {
                stream_.SkipChar(3);
                return MakeLiteral(TokenValue(std::stof(float_str)));
            } else if (stream_.CheckSeq(1, { '6', '4' })) {
                stream_.SkipChar(3);
                return MakeLiteral(TokenValue(std::stod(float_str)));
            }
        }

        if (is_dot_found || is_exponent_found) {
            if (is_exponent_found && is_dot_found && !is_digit_after_dot_found) {
                return MakeError("TODO");
            }
            return MakeLiteral(TokenValue(std::stod(float_str)));
        }
    }

    TokenValue token_value;
    if (!TryParse<uint8_t, uint16_t, uint32_t, uint64_t>(digits, &token_value)) { // TODO check u128
        return MakeError("TODO");
    }

    #define TP(type)                               \
        type result;                               \
        if (TryParse<type>(digits, &result)) {     \
            token_value = result;                  \
        } else {                                   \
            return MakeError("TODO"); \
        }

    #define TP_BRANCH(utype, itype) \
        if (c == 'u') {             \
            TP(utype)               \
        } else if (c == 'i') {      \
            TP(itype)               \
        }

    if (!stream_.IsEOF()) {
        if (stream_.CheckSeq(1, { '8' })) {
            stream_.SkipChar(2);
            TP_BRANCH(uint8_t, int8_t)
        } else if (stream_.CheckSeq(1, { '1', '6' })) {
            stream_.SkipChar(3);
            TP_BRANCH(uint16_t, int16_t)
        } else if (stream_.CheckSeq(1, { '3', '2' })) {
            stream_.SkipChar(3);
            TP_BRANCH(uint32_t, int32_t)
        } else if (stream_.CheckSeq(1, { '6', '4' })) {
            stream_.SkipChar(3);
            TP_BRANCH(uint64_t, int64_t)
        } else if (stream_.CheckSeq(1, { '1', '2', '8' })) {
            throw std::exception(); // TODO not implemented
        } else if (stream_.CheckSeq(1, { 's', 'i', 'z', 'e' })) {
            throw std::exception(); // TODO not implemented
        }
    }

    return MakeLiteral(token_value);
}
