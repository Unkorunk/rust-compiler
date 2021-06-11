#include "ErrorNode.hpp"

ErrorNode::ErrorNode(const std::string &error, Token::Position position) : error_(error), position_(position) {}

void ErrorNode::Visit(SyntaxTreeVisitor *visitor) const {
    visitor->PostVisit(this);
}

Token::Position ErrorNode::GetPosition() const {
    return position_;
}

std::string ErrorNode::GetError() const {
    return error_;
}
