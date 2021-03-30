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
        return TokenizeNumber();
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

    #define BracketsCheck(os, ot, cs, ct, stack)       \
        case os: {                                     \
            SkipChar(1);                               \
            stack.push(os);                            \
            return MakeToken(ot);                      \
        }                                              \
        case cs: {                                     \
            SkipChar(1);                               \
            if (stack.empty() || stack.top() != os) {  \
                return MakeToken(Token::Type::kError); \
            }                                          \
            stack.pop();                               \
            return MakeToken(ct);                      \
        }

    switch (c)
    {
    case '\'':
        if (CheckSeq(1, { 's', 't', 'a', 't', 'i', 'c' })) {
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
    BracketsCheck('{', Token::Type::kOpenCurlyBr, '}', Token::Type::kCloseCurlyBr, curly_braces_stack)
    BracketsCheck('[', Token::Type::kOpenSquareBr, ']', Token::Type::kCloseSquareBr, square_brackets_stack)
    BracketsCheck('(', Token::Type::kOpenRoundBr, ')', Token::Type::kCloseRoundBr, parentheses_stack)
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
    const std::map<std::string, Token::Type> keywords = {
        std::make_pair("as", Token::Type::kAs),
        std::make_pair("break", Token::Type::kBreak),
        std::make_pair("const", Token::Type::kConst),
        std::make_pair("continue", Token::Type::kContinue),
        std::make_pair("crate", Token::Type::kCrate),
        std::make_pair("else", Token::Type::kElse),
        std::make_pair("enum", Token::Type::kEnum),
        std::make_pair("extern", Token::Type::kExtern),
        std::make_pair("false", Token::Type::kFalse),
        std::make_pair("fn", Token::Type::kFn),
        std::make_pair("for", Token::Type::kFor),
        std::make_pair("if", Token::Type::kIf),
        std::make_pair("impl", Token::Type::kImpl),
        std::make_pair("in", Token::Type::kIn),
        std::make_pair("let", Token::Type::kLet),
        std::make_pair("loop", Token::Type::kLoop),
        std::make_pair("match", Token::Type::kMatch),
        std::make_pair("mod", Token::Type::kMod),
        std::make_pair("move", Token::Type::kMove),
        std::make_pair("mut", Token::Type::kMut),
        std::make_pair("pub", Token::Type::kPub),
        std::make_pair("ref", Token::Type::kRef),
        std::make_pair("return", Token::Type::kReturn),
        std::make_pair("self", Token::Type::kSelfValue),
        std::make_pair("Self", Token::Type::kSelfType),
        std::make_pair("static", Token::Type::kStatic),
        std::make_pair("struct", Token::Type::kStruct),
        std::make_pair("super", Token::Type::kSuper),
        std::make_pair("trait", Token::Type::kTrait),
        std::make_pair("true", Token::Type::kTrue),
        std::make_pair("type", Token::Type::kType),
        std::make_pair("unsafe", Token::Type::kUnsafe),
        std::make_pair("use", Token::Type::kUse),
        std::make_pair("where", Token::Type::kWhere),
        std::make_pair("while", Token::Type::kWhile),
        std::make_pair("async", Token::Type::kAsync),
        std::make_pair("await", Token::Type::kAwait),
        std::make_pair("dyn", Token::Type::kDyn),
        std::make_pair("abstract", Token::Type::kAbstract),
        std::make_pair("become", Token::Type::kBecome),
        std::make_pair("box", Token::Type::kBox),
        std::make_pair("do", Token::Type::kDo),
        std::make_pair("final", Token::Type::kFinal),
        std::make_pair("macro", Token::Type::kMacro),
        std::make_pair("override", Token::Type::kOverride),
        std::make_pair("priv", Token::Type::kPriv),
        std::make_pair("typeof", Token::Type::kTypeof),
        std::make_pair("unsized", Token::Type::kUnsized),
        std::make_pair("virtual", Token::Type::kVirtual),
        std::make_pair("yield", Token::Type::kYield),
        std::make_pair("try", Token::Type::kTry),
        std::make_pair("union", Token::Type::kUnion),
        // std::make_pair("'static", Token::Type::kStaticLifetime)
    };

    std::string keyword_buf;

    Token::Type best_match = Token::Type::kEmpty;

    while (true) {
        char_type c = PeekChar(0);
        keyword_buf += c;

        auto iter1 = keywords.lower_bound(keyword_buf);
        if (iter1 == keywords.end()) {
            if (best_match != Token::Type::kEmpty) {
                *token = MakeToken(best_match);
                return true;
            }
            return false;
        }

        auto mismatch_result = std::mismatch(keyword_buf.begin(), keyword_buf.end(), iter1->first.begin(), iter1->first.end());

        if (mismatch_result.first == keyword_buf.end()) {
            if (keyword_buf.size() == iter1->first.size()) {
                best_match = iter1->second;
            }
        } else {
            if (best_match != Token::Type::kEmpty) {
                *token = MakeToken(best_match);
                return true;
            }
            return false;
        }

        SkipChar(1);
    }
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
        return MakeToken(Token::Type::kError);
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
                return MakeToken(Token::Type::kError);
            }
        }

        if (c == 'f') {
            if (is_dot_found && !is_digit_after_dot_found) {
                return MakeToken(Token::Type::kError);
            }

            if (CheckSeq(1, { '3', '2' })) {
                SkipChar(3);
                return MakeToken(TokenValue(std::stof(float_str)));
            } else if (CheckSeq(1, { '6', '4' })) {
                SkipChar(3);
                return MakeToken(TokenValue(std::stod(float_str)));
            }
        }

        if (is_dot_found || is_exponent_found) {
            if (is_exponent_found && is_dot_found && !is_digit_after_dot_found) {
                return MakeToken(Token::Type::kError);
            }
            return MakeToken(TokenValue(std::stod(float_str)));
        }
    }

    TokenValue token_value;
    if (!TryParse<uint8_t, uint16_t, uint32_t, uint64_t>(digits, &token_value)) { // TODO check u128
        return MakeToken(Token::Type::kError);
    }

    #define TP(type)                               \
        type result;                               \
        if (TryParse<type>(digits, &result)) {     \
            token_value = result;                  \
        } else {                                   \
            return MakeToken(Token::Type::kError); \
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

    return MakeToken(token_value);
}

Token Tokenizer::TokenizeBoolean() {
    // TODO
    return MakeToken(Token::Type::kLiteral);
}
