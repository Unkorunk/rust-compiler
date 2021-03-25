#include "Tokenizer.hpp"

Tokenizer::Tokenizer(stream_type *stream) : stream_(stream) {}

bool Tokenizer::HasNext() const {
    return !IsEOF();
}

Token Tokenizer::Next() {
    SkipWhitespace();

    if (IsEOF()) {
        return Token(Token::Type::kEndOfFile);
    }

    char_type c = PeekChar(0);
    if (IsDecDigit(c)) {
        return TokenizeInteger();
    } else if (c != '_' && IsAlphanumeric(c)) {
        Token token(Token::Type::kEndOfFile);
        if (TryTokenizeKeyword(&token)) {
            return token;
        }
        return TokenizeIdentifier();
    }

    #define OpWithEq(symbol, op, opEq) \
        case symbol: {                 \
            c = PeekChar(1);           \
            if (c == '=') {            \
                SkipChar(2);           \
                return Token(opEq);    \
            }                          \
            SkipChar(1);               \
            return Token(op);          \
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
        return Token(Token::Type::kError);
    }
    case '-': {
        c = PeekChar(1);

        if (c == '=') {
            SkipChar(2);
            return Token(Token::Type::kMinusEq);
        } else if (c == '>') {
            SkipChar(2);
            return Token(Token::Type::kRArrow);
        }

        SkipChar(1);
        return Token(Token::Type::kMinus);
    }
    case '&': {
        c = PeekChar(1);

        if (c == '=') {
            SkipChar(2);
            return Token(Token::Type::kAndEq);
        } else if (c == '&') {
            SkipChar(2);
            return Token(Token::Type::kAndAnd);
        }

        SkipChar(1);
        return Token(Token::Type::kAnd);
    }
    case '|': {
        c = PeekChar(1);

        if (c == '=') {
            SkipChar(2);
            return Token(Token::Type::kOrEq);
        } else if (c == '|') {
            SkipChar(2);
            return Token(Token::Type::kOrOr);
        }

        SkipChar(1);
        return Token(Token::Type::kOr);
    }
    case '<': {
        c = PeekChar(1);

        if (c == '<') {
            c = PeekChar(2);

            if (c == '=') {
                SkipChar(3);
                return Token(Token::Type::kShlEq);
            }

            SkipChar(2);
            return Token(Token::Type::kShl);
        } else if (c == '=') {
            SkipChar(2);
            return Token(Token::Type::kLe);
        }

        SkipChar(1);
        return Token(Token::Type::kLt);
    }
    case '>': {
        c = PeekChar(1);

        if (c == '>') {
            c = PeekChar(2);

            if (c == '=') {
                SkipChar(3);
                return Token(Token::Type::kShrEq);
            }

            SkipChar(2);
            return Token(Token::Type::kShr);
        } else if (c == '=') {
            SkipChar(2);
            return Token(Token::Type::kGe);
        }

        SkipChar(1);
        return Token(Token::Type::kGt);
    }
    case '=': {
        c = PeekChar(1);

        if (c == '=') {
            SkipChar(2);
            return Token(Token::Type::kEqEq);
        } else if (c == '>') {
            SkipChar(2);
            return Token(Token::Type::kFatArrow);
        }

        SkipChar(1);
        return Token(Token::Type::kEq);
    }
    case '@': {
        SkipChar(1);
        return Token(Token::Type::kAt);
    }
    case '_': {
        SkipChar(1);
        return Token(Token::Type::kUnderscore);
    }
    case '.': {
        c = PeekChar(1);

        if (c == '.') {
            c = PeekChar(2);

            if (c == '.') {
                SkipChar(3);
                return Token(Token::Type::kDotDotDot);
            } else if (c == '=') {
                SkipChar(3);
                return Token(Token::Type::kDotDotEq);
            }

            SkipChar(2);
            return Token(Token::Type::kDotDot);
        }

        SkipChar(1);
        return Token(Token::Type::kDot);
    }
    case ',': {
        SkipChar(1);
        return Token(Token::Type::kComma);
    }
    case ';': {
        SkipChar(1);
        return Token(Token::Type::kSemi);
    }
    case ':': {
        c = PeekChar(1);

        if (c == ':') {
            SkipChar(2);
            return Token(Token::Type::kPathSep);
        }

        SkipChar(1);
        return Token(Token::Type::kColon);
    }
    case '#': {
        SkipChar(1);
        return Token(Token::Type::kPound);
    }
    case '$': {
        SkipChar(1);
        return Token(Token::Type::kDollar);
    }
    case '?': {
        SkipChar(1);
        return Token(Token::Type::kQuestion);
    }
    OpWithEq('+', Token::Type::kPlus, Token::Type::kPlusEq)
    OpWithEq('*', Token::Type::kStar, Token::Type::kStarEq)
    OpWithEq('/', Token::Type::kSlash, Token::Type::kSlashEq)
    OpWithEq('%', Token::Type::kPercent, Token::Type::kPercentEq)
    OpWithEq('^', Token::Type::kCaret, Token::Type::kCaretEq)
    OpWithEq('!', Token::Type::kNot, Token::Type::kNe)
    default:
        SkipChar(1);
        return Token(Token::Type::kError);
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
    stream_->ignore(offset);
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
    return Token(Token::Type::kIdentifier);
}

Token Tokenizer::TokenizeCharacter() {
    // TODO
    return Token(Token::Type::kLiteral);
}

Token Tokenizer::TokenizeString() {
    // TODO
    return Token(Token::Type::kLiteral);
}

Token Tokenizer::TokenizeRawString() {
    // TODO
    return Token(Token::Type::kLiteral);
}

Token Tokenizer::TokenizeByte() {
    // TODO
    return Token(Token::Type::kLiteral);
}

Token Tokenizer::TokenizeByteString() {
    // TODO
    return Token(Token::Type::kLiteral);
}

Token Tokenizer::TokenizeRawByteString() {
    // TODO
    return Token(Token::Type::kLiteral);
}

Token Tokenizer::TokenizeInteger() {
    long long result = 0;
    bool is_digit_found = false;

    char_type c = PeekChar(0);

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
        } else if (IsDecDigit(c)) {
            system = 10;
            is_digit_found = true;
        } else {
            return Token(Token::Type::kLiteral);
        }
    }

    while (!IsEOF()) {
        if (system == 2 && IsBinDigit(c) || system == 8 && IsOctDigit(c) ||
            system == 10 && IsDecDigit(c)) {
            result *= 10;

            result += c - '0';

            is_digit_found = true;
        } else if (system == 16 && IsHexDigit(c)) {
            result *= 10;

            if (c >= '0' && c <= '9') {
                result += c - '0';
            } else if (c >= 'a' && c <= 'f') {
                result += 10 + (c - 'a');
            } else {
                result += 10 + (c - 'A');
            }

            is_digit_found = true;
        } else if (c != '_') {
            break;
        }

        SkipChar(1);
        c = PeekChar(0);
    }

    if (!is_digit_found) {
        return Token(Token::Type::kError);
    }

    return Token(Token::Type::kLiteral);
}

Token Tokenizer::TokenizeFloat() {
    // TODO
    return Token(Token::Type::kLiteral);
}

Token Tokenizer::TokenizeBoolean() {
    // TODO
    return Token(Token::Type::kLiteral);
}
