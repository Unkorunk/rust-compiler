#include "BlockNode.hpp"

void BlockNode::Visit(SyntaxTreeVisitor *visitor) const {
    visitor->PostVisit(this);
}
