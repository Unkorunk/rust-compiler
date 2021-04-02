#pragma once

#include <fstream>

class InputStream {
public:
    explicit InputStream(std::ifstream *stream) : stream_(stream) {}

    bool IsEOF() const {
        return stream_->eof();
    }

    char PeekChar(std::streamoff offset) {
        std::streampos old_pos = stream_->tellg();
        
        stream_->seekg(offset, std::ios_base::cur);
        int c = stream_->peek();
        stream_->seekg(old_pos);

        return (c == std::char_traits<char>::eof() ? ' ' : c);
    }

    void SkipChar(std::streamsize offset) {
        while (offset != 0 && !IsEOF()) {
            NextChar();
            offset--;
        }
    }

    char NextChar() {
        int c = stream_->get();

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

    uint32_t GetStartLine() const {
        return start_line_;
    }

    uint32_t GetStartColumn() const {
        return start_column_;
    }

    void AssignStart() {
        start_line_ = current_line_;
        start_column_ = current_column_;
    }

private:
    std::ifstream *stream_;

    uint32_t current_line_ = 1, current_column_ = 1;
    uint32_t start_line_ = 1, start_column_ = 1;
    const uint32_t tab_size_ = 4;
    
};
