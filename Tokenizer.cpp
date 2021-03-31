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
    char_type c1 = PeekChar(1);
    char_type c2 = PeekChar(2);
    if (IsDecDigit(c)) {
        return TokenizeNumber();
    } else if (c == '_' && IsAlphanumeric(PeekChar(1)) ||
        c >= 'a' && c <= 'z' && c != 'r' && c != 'b' ||
        c >= 'A' && c <= 'Z' ||
        c == 'r' && (c1 != '"' && (c1 != '#' || c2 != '"' && c2 != '#')) || //! raw string literals
        c == 'b' && c1 != '\'' && c1 != '"' && (c1 != 'r' || c2 != '"' && c2 != '#')) { //! byte and byte string literals
        return TokenizeIdentifierOrKeyword();
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

    #define BracketsCheck(os, ot, cs, ct, stack)       \
        case os: {                                     \
            SkipChar(1);                               \
            stack.push(os);                            \
            return MakeToken(ot);                      \
        }                                              \
        case cs: {                                     \
            SkipChar(1);                               \
            if (stack.empty() || stack.top() != os) {  \
                return MakeError("TODO");              \
            }                                          \
            stack.pop();                               \
            return MakeToken(ct);                      \
        }

    #define SingleCheck(symbol, token) \
        case symbol:                   \
            SkipChar(1);               \
            return MakeToken(token);   \

    switch (c)
    {
    case '\'':
        if (CheckSeq(1, { 's', 't', 'a', 't', 'i', 'c' })) { //! !static - keyword
            SkipChar(7);
            return MakeToken(Token::Type::kStaticLifetime);
        }
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
        return MakeError("TODO");
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
    case ':': {
        c = PeekChar(1);

        if (c == ':') {
            SkipChar(2);
            return MakeToken(Token::Type::kPathSep);
        }

        SkipChar(1);
        return MakeToken(Token::Type::kColon);
    }
    case '/': {
        c = PeekChar(1);
        if (c == '=') {
            SkipChar(2);
            return MakeToken(Token::Type::kSlashEq);
        } else if (c == '/') {
            SkipLineComment();
            return Next();
        } else if (c == '*') {
            SkipMultilineComment();
            return Next();
        }
        SkipChar(1);
        return MakeToken(Token::Type::kSlash);
    }
    SingleCheck('@', Token::Type::kAt)
    SingleCheck('_', Token::Type::kUnderscore)
    SingleCheck(',', Token::Type::kComma)
    SingleCheck(';', Token::Type::kSemi)
    SingleCheck('#', Token::Type::kPound)
    SingleCheck('$', Token::Type::kDollar)
    SingleCheck('?', Token::Type::kQuestion)
    OpWithEq('+', Token::Type::kPlus, Token::Type::kPlusEq)
    OpWithEq('*', Token::Type::kStar, Token::Type::kStarEq)
    OpWithEq('%', Token::Type::kPercent, Token::Type::kPercentEq)
    OpWithEq('^', Token::Type::kCaret, Token::Type::kCaretEq)
    OpWithEq('!', Token::Type::kNot, Token::Type::kNe)
    BracketsCheck('{', Token::Type::kOpenCurlyBr, '}', Token::Type::kCloseCurlyBr, curly_braces_stack)
    BracketsCheck('[', Token::Type::kOpenSquareBr, ']', Token::Type::kCloseSquareBr, square_brackets_stack)
    BracketsCheck('(', Token::Type::kOpenRoundBr, ')', Token::Type::kCloseRoundBr, parentheses_stack)
    default:
        SkipChar(1);
        return MakeError("TODO");
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
        NextChar();
        offset--;
    }
}

Tokenizer::char_type Tokenizer::NextChar() {
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

    return c;
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

void Tokenizer::SkipLineComment() {
    SkipChar(2);
    while(!IsEOF() && PeekChar(0) != '\n') {
        SkipChar(1);
    }
}

void Tokenizer::SkipMultilineComment() {
    SkipChar(2);

    bool is_start_found = false;

    std::stack<bool> comment_stack;
    comment_stack.push(1);
    while (!IsEOF() && !comment_stack.empty()) {
        char_type c = PeekChar(0);
        if (c == '/' && PeekChar(1) == '*') {
            comment_stack.push(1);
        } else if (c == '*') {
            is_start_found = true;
        } else if (c == '/' && is_start_found) {
            if (comment_stack.empty() || comment_stack.top() != 1) {
                SkipChar(1);
                throw std::exception(); // TODO PushError("blabla")
            }
            comment_stack.pop();
        } else {
            is_start_found = false;
        }
        SkipChar(1);
    }
}

Token Tokenizer::TokenizeIdentifierOrKeyword() {
    bool is_raw_identifier = false;

    char_type c = PeekChar(0);
    if (c == 'r' && PeekChar(1) == '#') {
        is_raw_identifier = true;
        SkipChar(2);
        c = PeekChar(0);
    }

    std::string identifier_buf;
    identifier_buf += c;

    if (c == '_') {
        SkipChar(1);
        c = PeekChar(0);
        if (!IsAlphanumeric(c)) {
            return MakeError("TODO");
        }
    } else if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z') {
        SkipChar(1);
        c = PeekChar(0);
    } else {
        return MakeError("TODO");
    }

    while (IsAlphanumeric(c)) {
        identifier_buf += c;
        SkipChar(1);
        c = PeekChar(0);
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
        return MakeError("TODO");
    }

    if (system == 10 && !IsEOF()) {
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

            SkipChar(1);
            c = PeekChar(0);
            while (IsDecDigit(c)) {
                is_digit_after_dot_found = true;
                float_str += c;
                SkipChar(1);
                c = PeekChar(0);
            }
        }

        if (c == 'e' || c == 'E') {
            bool is_digit_after_exponent_found = false;

            is_exponent_found = true;
            float_str += 'e';

            SkipChar(1);
            c = PeekChar(0);
            if (c == '+' || c == '-') {
                float_str += c;
            }

            SkipChar(1);
            c = PeekChar(0);
            while(IsDecDigit(c) || c == '_') {
                if (c != '_') {
                    is_digit_after_exponent_found = true;
                    float_str += c;
                }
                SkipChar(1);
                c = PeekChar(0);
            }

            if (!is_digit_after_exponent_found) {
                return MakeError("TODO");
            }
        }

        if (c == 'f') {
            if (is_dot_found && !is_digit_after_dot_found) {
                return MakeError("TODO");
            }

            if (CheckSeq(1, { '3', '2' })) {
                SkipChar(3);
                return MakeLiteral(TokenValue(std::stof(float_str)));
            } else if (CheckSeq(1, { '6', '4' })) {
                SkipChar(3);
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

    if (!IsEOF()) {
        if (CheckSeq(1, { '8' })) {
            SkipChar(2);
            TP_BRANCH(uint8_t, int8_t)
        } else if (CheckSeq(1, { '1', '6' })) {
            SkipChar(3);
            TP_BRANCH(uint16_t, int16_t)
        } else if (CheckSeq(1, { '3', '2' })) {
            SkipChar(3);
            TP_BRANCH(uint32_t, int32_t)
        } else if (CheckSeq(1, { '6', '4' })) {
            SkipChar(3);
            TP_BRANCH(uint64_t, int64_t)
        } else if (CheckSeq(1, { '1', '2', '8' })) {
            throw std::exception(); // TODO not implemented
        } else if (CheckSeq(1, { 's', 'i', 'z', 'e' })) {
            throw std::exception(); // TODO not implemented
        }
    }

    return MakeLiteral(token_value);
}
