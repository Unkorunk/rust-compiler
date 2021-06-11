#include "StructNode.hpp"

StructNode::StructNode(IdentifierNode *identifier_node, const std::vector<RawParam> &params) 
	: identifier_node(identifier_node) {
    this->params.reserve(params.size());
    for (const RawParam &param : params) {
        this->params.emplace_back(param);
    }
}

void StructNode::Visit(SyntaxTreeVisitor *visitor) const {
    visitor->PostVisit(this);
}
