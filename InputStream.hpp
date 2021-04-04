#pragma once

#include <fstream>

class InputStream {
public:
    explicit InputStream(std::ifstream *stream) : stream_(stream), current_offset_(stream->tellg()) {}

    bool IsEOF() const {
        return stream_->eof() || is_eof_;
    }

    char PeekChar(std::streamoff offset) {
        std::streampos old_pos = stream_->tellg();
        current_offset_ = old_pos;
        
        stream_->seekg(offset, std::ios_base::cur);
        int c = stream_->peek();
        stream_->seekg(old_pos);

        if (c == std::char_traits<char>::eof()) {
            is_eof_ = true;
            return ' ';
        } else {
            is_eof_ = false;
        }

        return c;
    }

    void SkipChar(std::streamsize offset) {
        while (offset != 0 && !IsEOF()) {
            NextChar();
            offset--;
        }
    }

    char NextChar() {
        int c = stream_->get();
        current_offset_ = stream_->tellg();

        if (c == '\t') {
            current_column_ += tab_size_;
        } else if (c == '\n' || c == '\f') {
            current_line_++;
            current_column_ = 1;
        } else if (c == '\v') {
            current_line_++;
        } else {
            current_column_++;
        }

        return c;
    }

    bool CheckSeq(std::streamoff offset, const std::initializer_list<char>& seq) {
        for (char it : seq) {
            if (IsEOF() || PeekChar(offset++) != it) {
                return false;
            }
        }

        return true;
    }

    uint32_t GetCurrentLine() const {
        return current_line_;
    }

    uint32_t GetCurrentColumn() const {
        return current_column_;
    }

    std::streampos GetCurrentOffset() const {
        return current_offset_;
    }

    uint32_t GetStartLine() const {
        return start_line_;
    }

    uint32_t GetStartColumn() const {
        return start_column_;
    }

    void AssignStart() {
        start_line_ = current_line_;
        start_column_ = current_column_;
        start_offset_ = current_offset_;
    }

    std::streampos GetStartOffset() const {
        return start_offset_;
    }

    Token::Position GetTokenPosition() const {
        return Token::Position(
            GetStartLine(),
            GetStartColumn(),
            GetStartOffset(),
            GetCurrentLine(),
            GetCurrentColumn(),
            GetCurrentOffset()
        );
    }

    operator std::ifstream *() {
        return stream_;
    }

private:
    std::ifstream *stream_;

    uint32_t current_line_ = 1, current_column_ = 1;
    std::streampos current_offset_;

    uint32_t start_line_ = 1, start_column_ = 1;
    std::streampos start_offset_;

    const uint32_t tab_size_ = 4;

    bool is_eof_ = false;
    
};
