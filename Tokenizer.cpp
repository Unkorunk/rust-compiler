#include "Tokenizer.hpp"

Tokenizer::Tokenizer(istream_type *stream) : stream_(stream) {}

bool Tokenizer::HasNext() const {
    return false;
}

Token Tokenizer::Next() {
    SkipWhitespace();

    if (!IsEOF()) {
        return Token(Token::Type::kEndOfFile);
    }

    char_type c = PeekChar(0);
    if (IsDecDigit(c)) {
        return TokenizeInteger();
    } else if (IsAlphanumeric(c)) {
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
                return Token(op);      \
            }                          \
            SkipChar(1);               \
            return Token(opEq);        \
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
        } else if (c == '&') {
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
    return !stream_->eof();
}

Tokenizer::char_type Tokenizer::PeekChar(size_t offset) {
    if (offset >= buf2_.size()) {
        throw std::exception();
    }

    if (buf1_position_ >= buf1_length_) {
        if (buf2_initialized_) {
            buf1_.swap(buf2_);
            buf2_initialized_ = false;
        } else {
            stream_->read(buf1_.data(), buf1_.size());
            buf1_length_ = stream_->gcount();
            buf1_position_ = 0;
        }
    }

    if (buf1_position_ + offset >= buf1_length_) {
        if (!buf2_initialized_) {
            stream_->read(buf2_.data(), buf2_.size());
            buf2_length_ = stream_->gcount();

            buf2_initialized_ = true;
        }

        if (buf1_position_ + offset < buf1_length_ + buf2_length_) {
            return buf2_[buf1_position_ + offset - buf1_length_];
        }
    } else {
        return buf1_[buf1_position_ + offset];
    }

    return ' ';
}

void Tokenizer::SkipChar(size_t offset) {
    buf1_position_ += offset;
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
    while (IsEOF() && IsWhitespace(PeekChar(0))) {
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
    // TODO
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
