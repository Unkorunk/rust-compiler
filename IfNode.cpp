
#include "IfNode.hpp"

#include "IfNode.h"

void IfNode::Visit(SyntaxTreeVisitor *visitor) const {
    visitor->PostVisit(this);
}
