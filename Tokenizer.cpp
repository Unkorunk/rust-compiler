#include "Tokenizer.hpp"

Tokenizer::Tokenizer(stream_type *stream) : stream_(stream) {}

bool Tokenizer::HasNext() const {
    return !IsEOF();
}

Token Tokenizer::Next() {
    SkipWhitespace();

    if (IsEOF()) {
        return MakeToken(Token::Type::kEndOfFile);
    }

    AssignStart();

    char_type c = PeekChar(0);
    if (IsDecDigit(c)) {
        return TokenizeInteger();
    } else if (c != '_' && IsAlphanumeric(c)) {
        Token token;
        if (TryTokenizeKeyword(&token)) {
            return token;
        }
        return TokenizeIdentifier();
    }

    #define OpWithEq(symbol, op, opEq)  \
        case symbol: {                  \
            c = PeekChar(1);            \
            if (c == '=') {             \
                SkipChar(2);            \
                return MakeToken(opEq); \
            }                           \
            SkipChar(1);                \
            return MakeToken(op);       \
        }

    switch (c)
    {
    case '\'':
        return TokenizeCharacter();
    case '"':
        return TokenizeString();
    case 'r':
        return TokenizeRawString();
    case 'b': {
        c = PeekChar(1);

        if (c == '\'') {
            SkipChar(2);
            return TokenizeByte();
        } else if (c == '"') {
            SkipChar(2);
            return TokenizeByteString();
        } else if (c == 'r') {
            SkipChar(2);
            return TokenizeRawByteString();
        }

        SkipChar(1);
        return MakeToken(Token::Type::kError);
    }
    case '-': {
        c = PeekChar(1);

        if (c == '=') {
            SkipChar(2);
            return MakeToken(Token::Type::kMinusEq);
        } else if (c == '>') {
            SkipChar(2);
            return MakeToken(Token::Type::kRArrow);
        }

        SkipChar(1);
        return MakeToken(Token::Type::kMinus);
    }
    case '&': {
        c = PeekChar(1);

        if (c == '=') {
            SkipChar(2);
            return MakeToken(Token::Type::kAndEq);
        } else if (c == '&') {
            SkipChar(2);
            return MakeToken(Token::Type::kAndAnd);
        }

        SkipChar(1);
        return MakeToken(Token::Type::kAnd);
    }
    case '|': {
        c = PeekChar(1);

        if (c == '=') {
            SkipChar(2);
            return MakeToken(Token::Type::kOrEq);
        } else if (c == '|') {
            SkipChar(2);
            return MakeToken(Token::Type::kOrOr);
        }

        SkipChar(1);
        return MakeToken(Token::Type::kOr);
    }
    case '<': {
        c = PeekChar(1);

        if (c == '<') {
            c = PeekChar(2);

            if (c == '=') {
                SkipChar(3);
                return MakeToken(Token::Type::kShlEq);
            }

            SkipChar(2);
            return MakeToken(Token::Type::kShl);
        } else if (c == '=') {
            SkipChar(2);
            return MakeToken(Token::Type::kLe);
        }

        SkipChar(1);
        return MakeToken(Token::Type::kLt);
    }
    case '>': {
        c = PeekChar(1);

        if (c == '>') {
            c = PeekChar(2);

            if (c == '=') {
                SkipChar(3);
                return MakeToken(Token::Type::kShrEq);
            }

            SkipChar(2);
            return MakeToken(Token::Type::kShr);
        } else if (c == '=') {
            SkipChar(2);
            return MakeToken(Token::Type::kGe);
        }

        SkipChar(1);
        return MakeToken(Token::Type::kGt);
    }
    case '=': {
        c = PeekChar(1);

        if (c == '=') {
            SkipChar(2);
            return MakeToken(Token::Type::kEqEq);
        } else if (c == '>') {
            SkipChar(2);
            return MakeToken(Token::Type::kFatArrow);
        }

        SkipChar(1);
        return MakeToken(Token::Type::kEq);
    }
    case '@': {
        SkipChar(1);
        return MakeToken(Token::Type::kAt);
    }
    case '_': {
        SkipChar(1);
        return MakeToken(Token::Type::kUnderscore);
    }
    case '.': {
        c = PeekChar(1);

        if (c == '.') {
            c = PeekChar(2);

            if (c == '.') {
                SkipChar(3);
                return MakeToken(Token::Type::kDotDotDot);
            } else if (c == '=') {
                SkipChar(3);
                return MakeToken(Token::Type::kDotDotEq);
            }

            SkipChar(2);
            return MakeToken(Token::Type::kDotDot);
        }

        SkipChar(1);
        return MakeToken(Token::Type::kDot);
    }
    case ',': {
        SkipChar(1);
        return MakeToken(Token::Type::kComma);
    }
    case ';': {
        SkipChar(1);
        return MakeToken(Token::Type::kSemi);
    }
    case ':': {
        c = PeekChar(1);

        if (c == ':') {
            SkipChar(2);
            return MakeToken(Token::Type::kPathSep);
        }

        SkipChar(1);
        return MakeToken(Token::Type::kColon);
    }
    case '#': {
        SkipChar(1);
        return MakeToken(Token::Type::kPound);
    }
    case '$': {
        SkipChar(1);
        return MakeToken(Token::Type::kDollar);
    }
    case '?': {
        SkipChar(1);
        return MakeToken(Token::Type::kQuestion);
    }
    OpWithEq('+', Token::Type::kPlus, Token::Type::kPlusEq)
    OpWithEq('*', Token::Type::kStar, Token::Type::kStarEq)
    OpWithEq('/', Token::Type::kSlash, Token::Type::kSlashEq)
    OpWithEq('%', Token::Type::kPercent, Token::Type::kPercentEq)
    OpWithEq('^', Token::Type::kCaret, Token::Type::kCaretEq)
    OpWithEq('!', Token::Type::kNot, Token::Type::kNe)
    default:
        SkipChar(1);
        return MakeToken(Token::Type::kError);
    }
}

bool Tokenizer::IsEOF() const {
    return stream_->eof();
}

Tokenizer::char_type Tokenizer::PeekChar(std::streamoff offset) {
    std::streampos old_pos = stream_->tellg();
    
    stream_->seekg(offset, std::ios_base::cur);
    char_type c = stream_->peek();
    stream_->seekg(old_pos);

    return (c == std::char_traits<char_type>::eof() ? ' ' : c);
}

void Tokenizer::SkipChar(std::streamsize offset) {
    while (offset != 0 && !IsEOF()) {
        char_type c = stream_->get();

        if (c == '\t') {
            current_column_ += tab_size_;
        } else if (c == '\n') {
            current_line_++;
            current_column_ = 1;
        } else if (c == '\v') {
            current_line_++;
        } else {
            current_column_++;
        }
        // TODO not implemented other whitespace

        offset--;
    }
}

bool Tokenizer::CheckSeq(std::streamoff offset, const std::initializer_list<char_type>& seq) {
    for (char_type it : seq) {
        if (IsEOF() || PeekChar(offset++) != it) {
            return false;
        }
    }

    return true;
}

bool Tokenizer::IsWhitespace(char_type it) {
    return it == '\t' || it == '\n' || it == '\v' ||
        it == '\f' || it == '\r' || it == ' ' ||
        it == '\u0085' || it == '\u200E' || it == '\u200F' ||
        it == '\u2028' || it == '\u2029';
}

bool Tokenizer::IsAlphanumeric(char_type it) {
    return it >= 'a' && it <= 'z' ||
        it >= 'A' && it <= 'Z' ||
        it >= '0' && it <= '9' ||
        it == '_';
}

bool Tokenizer::IsBinDigit(char_type it) {
    return it == '0' && it == '1';
}

bool Tokenizer::IsOctDigit(char_type it) {
    return it >= '0' && it <= '7';
}

bool Tokenizer::IsDecDigit(char_type it) {
    return it >= '0' && it <= '9';
}

bool Tokenizer::IsHexDigit(char_type it) {
    return IsDecDigit(it) || it >= 'a' && it <= 'f' ||
        it >= 'A' && it <= 'F';
}

void Tokenizer::SkipWhitespace() {
    while (!IsEOF() && IsWhitespace(PeekChar(0))) {
        SkipChar(1);
    }
}

bool Tokenizer::TryTokenizeKeyword(Token *token) {
    // TODO
    return false;
}

Token Tokenizer::TokenizeIdentifier() {
    // TODO
    return MakeToken(Token::Type::kIdentifier);
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

Token Tokenizer::TokenizeInteger() {
    bool is_digit_found = false;

    char_type c = PeekChar(0);
    if (!IsDecDigit(c)) {
        throw std::exception();
    }

    int system = (c != '0' ? 10 : -1);
    if (system == -1) {
        SkipChar(1);
        c = PeekChar(0);
        
        if (c == 'b') {
            system = 2;
            SkipChar(1);
            c = PeekChar(0);
        } else if (c == 'o') {
            system = 8;
            SkipChar(1);
            c = PeekChar(0);
        } else if (c == 'x') {
            system = 16;
            SkipChar(1);
            c = PeekChar(0);
        } else {
            system = 10;
            is_digit_found = true;
        }
    }

    std::vector<int8_t> digits;

    do {
        if (system == 2 && IsBinDigit(c) || system == 8 && IsOctDigit(c) ||
            system == 10 && IsDecDigit(c)) {
            digits.push_back(c - '0');
            is_digit_found = true;
        } else if (system == 16 && IsHexDigit(c)) {
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

        SkipChar(1);
        c = PeekChar(0);
    } while (!IsEOF());

    if (!is_digit_found) {
        return MakeToken(Token::Type::kError);
    }

    TokenValue token_value;
    uint8_t tmp1;
    if (!TryParse<uint8_t>(digits, &tmp1)) {
        uint16_t tmp2;
        if (!TryParse<uint16_t>(digits, &tmp2)) {
            uint32_t tmp3;
            if (!TryParse<uint32_t>(digits, &tmp3)) {
                uint64_t tmp4;
                if (!TryParse<uint64_t>(digits, &tmp4)) {
                    return MakeToken(Token::Type::kError); // TODO check u128
                } else {
                    token_value = tmp4;
                }
            } else {
                token_value = tmp3;
            }
        } else {
            token_value = tmp2;
        }
    } else {
        token_value = tmp1;
    }

    if (!IsEOF()) {
        int is_unsigned = -1;

        if (c == 'u') {
            is_unsigned = 1;
        } else if (c == 'i') {
            is_unsigned = 0;
        }

        if (is_unsigned != -1) {
            if (CheckSeq(1, { '8' })) {
                SkipChar(2);
                if (is_unsigned) {
                    uint8_t result;
                    if (TryParse<uint8_t>(digits, &result)) {
                        token_value = result;
                    } else {
                        return MakeToken(Token::Type::kError);
                    }
                } else {
                    int8_t result;
                    if (TryParse<int8_t>(digits, &result)) {
                        token_value = result;
                    } else {
                        return MakeToken(Token::Type::kError);
                    }
                }
            } else if (CheckSeq(1, { '1', '6' })) {
                SkipChar(3);
                if (is_unsigned) {
                    uint16_t result;
                    if (TryParse<uint16_t>(digits, &result)) {
                        token_value = result;
                    } else {
                        return MakeToken(Token::Type::kError);
                    }
                } else {
                    int16_t result;
                    if (TryParse<int16_t>(digits, &result)) {
                        token_value = result;
                    } else {
                        return MakeToken(Token::Type::kError);
                    }
                }
            } else if (CheckSeq(1, { '3', '2' })) {
                SkipChar(3);
                if (is_unsigned) {
                    uint32_t result;
                    if (TryParse<uint32_t>(digits, &result)) {
                        token_value = result;
                    } else {
                        return MakeToken(Token::Type::kError);
                    }
                } else {
                    int32_t result;
                    if (TryParse<int32_t>(digits, &result)) {
                        token_value = result;
                    } else {
                        return MakeToken(Token::Type::kError);
                    }
                }
            } else if (CheckSeq(1, { '6', '4' })) {
                SkipChar(3);
                if (is_unsigned) {
                    uint64_t result;
                    if (TryParse<uint64_t>(digits, &result)) {
                        token_value = result;
                    } else {
                        return MakeToken(Token::Type::kError);
                    }
                } else {
                    int64_t result;
                    if (TryParse<int64_t>(digits, &result)) {
                        token_value = result;
                    } else {
                        return MakeToken(Token::Type::kError);
                    }
                }
            } else if (CheckSeq(1, { '1', '2', '8' })) {
                throw std::exception(); // TODO not implemented
            } else if (CheckSeq(1, { 's', 'i', 'z', 'e' })) {
                throw std::exception(); // TODO not implemented
            }
        }
    }

    return MakeToken(token_value);
}

Token Tokenizer::TokenizeFloat() {
    // TODO
    return MakeToken(Token::Type::kLiteral);
}

Token Tokenizer::TokenizeBoolean() {
    // TODO
    return MakeToken(Token::Type::kLiteral);
}
