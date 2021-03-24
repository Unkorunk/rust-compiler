#include "Tokenizer.hpp"

Tokenizer::Tokenizer(std::istream *stream) : stream_(stream) {}

bool Tokenizer::HasNext() const {
    return false;
}

Token Tokenizer::Next() {
    return Token(Token::Type::kEndOfFile);
}

bool Tokenizer::HasNextChar() const {
    return !stream_->eof();
}

std::istream::char_type Tokenizer::NextChar() {
    if (buf_position_ >= buf_length_) {
        stream_->read(buf_.data(), buf_.size());
        buf_length_ = stream_->gcount();
        buf_position_ = 0;
    }

    if (buf_position_ < buf_length_) {
        return buf_[buf_position_++];
    }

    return ' ';
}
