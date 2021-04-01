#pragma once

#include <vector>
#include <functional>
#include <fstream>

#include "Token.hpp"
#include "InputStream.hpp"

class Punctuation {
public:
    using selector_type = std::function<Token()>;

    Punctuation(char start_symbol, Token::Type type)
        : start_symbol_(start_symbol), type_(type) {}

    Punctuation(char start_symbol, Token::Type type, const std::vector<Punctuation>& children)
        : Punctuation(start_symbol, type) { children_ = children; }

    Punctuation(char start_symbol, selector_type selector)
        : start_symbol_(start_symbol), selector_(selector) {}

    Punctuation(char start_symbol, selector_type selector, const std::vector<Punctuation>& children) 
        : Punctuation(start_symbol, selector) { children_ = children; }

    bool TryTokenize(InputStream *stream, Token *token) const {
        std::streamoff max_offset = 0;
        const Punctuation* punctuation = nullptr;

        TryTokenize(stream, &punctuation, 0, &max_offset);
        if (punctuation != nullptr) {
            stream->SkipChar(max_offset + 1);
            if (punctuation->selector_) {
                *token = punctuation->selector_();
            } else {
                *token = Token(punctuation->type_, stream->GetStartLine(), stream->GetStartColumn(),
                stream->GetCurrentLine(), stream->GetCurrentColumn());
            }

            return true;
        }
        return false;
    }

private:
    char start_symbol_;
    Token::Type type_;
    selector_type selector_;
    std::vector<Punctuation> children_;

    void TryTokenize(InputStream *stream, const Punctuation **punctuation, std::streamoff offset, std::streamoff *max_offset) const {
        char c = stream->PeekChar(offset);
        if (start_symbol_ != c) return;

        bool is_child = false;
        for (const Punctuation& child : children_) {
            child.TryTokenize(stream, punctuation, offset + 1, max_offset);
        }

        if (c == start_symbol_ && offset >= *max_offset) {
            *max_offset = offset;
            *punctuation = this;
        }
    }

};
