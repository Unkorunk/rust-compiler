#include "ErrorNode.hpp"

ErrorNode::ErrorNode(const std::string &error, Token::Position position) : error_(error), position_(position) {}

Token::Position ErrorNode::GetPosition() const {
    return position_;
}

std::string ErrorNode::GetError() const {
    return error_;
}
