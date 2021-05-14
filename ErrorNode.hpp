#pragma once

#include <string>

#include "SyntaxNode.hpp"
#include "SyntaxTreeVisitor.hpp"
#include "Token.hpp"

class ErrorNode : public SyntaxNode {
public:
    ErrorNode(const std::string& error, Token::Position position);

    void Visit(SyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

    std::string GetError() const;

    Token::Position GetPosition() const;

private:
    std::string error_;
    Token::Position position_;

};
