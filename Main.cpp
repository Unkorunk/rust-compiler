#include <fstream>
#include <iostream>

#include "SpecificSyntaxTreeVisitor.hpp"
#include "SyntaxParser.hpp"
#include "Tokenizer.hpp"

class MyVisitor : public SpecificSyntaxTreeVisitor {
protected:
    int indent = 0;
    void PrintIndent() const {
        for (int i = 0; i < indent; i++) {
            std::cout << ' ';
        }
    }

    void PostVisit(const IdentifierNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << node->GetToken()->GetTokenValue().ToString() << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const LiteralNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << node->GetToken()->GetTokenValue().ToString() << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const ParamFunctionNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "ParamFunctionNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const ParamStructNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "ParamStructNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const LetNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "LetNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const FunctionNode *node) override {
        indent += 2;

        PrintIndent();
        if (node->IsConst()) {
            std::cout << "const ";
        }
        std::cout << "FunctionNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const StructNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "StructNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const ConstantItemNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "ConstantItemNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const ParenthesizedTypeNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "ParenthesizedTypeNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const TupleTypeNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "TupleTypeNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const ReferenceTypeNode *node) override {
        indent += 2;

        PrintIndent();
        if (node->IsMut()) {
            std::cout << "mut ";
        }
        std::cout << "ReferenceTypeNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const ArrayTypeNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "ArrayTypeNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const IdentifierTypeNode *node) override {
        SpecificSyntaxTreeVisitor::PostVisit(node);
    }

    void PostVisit(const TupleIndexFieldNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "TupleIndexFieldNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const IdentifierFieldNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "IdentifierFieldNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const RefMutIdentifierFieldNode *node) override {
        indent += 2;

        PrintIndent();
        if (node->IsRef()) {
            std::cout << "ref ";
        }
        if (node->IsMut()) {
            std::cout << "mut ";
        }
        std::cout << "RefMutIdentifierFieldNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const LiteralPatternNode *node) override {
        SpecificSyntaxTreeVisitor::PostVisit(node);
    }

    void PostVisit(const IdentifierPatternNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "IdentifierPatternNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const WildcardPatternNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "WildcardPatternNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const RestPatternNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "RestPatternNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const ReferencePatternNode *node) override {
        indent += 2;

        PrintIndent();
        if (node->IsMut()) {
            std::cout << "mut ";
        }
        if (node->IsSingleRef()) {
            std::cout << "& ";
        } else {
            std::cout << "&& ";
        }
        std::cout << "ReferencePatternNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const StructPatternNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "StructPatternNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const TupleStructPatternNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "TupleStructPatternNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const TuplePatternNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "TuplePatternNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const GroupedPatternNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "GroupedPatternNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const IdentifierExpressionNode *node) override {
        SpecificSyntaxTreeVisitor::PostVisit(node);
    }

    void PostVisit(const LiteralExpressionNode *node) override {
        SpecificSyntaxTreeVisitor::PostVisit(node);
    }

    void PostVisit(const BinaryOperationNode *node) override {
        indent += 2;
        const auto token = node->GetToken();
        const auto tokenType = token->GetType();

        PrintIndent();
        std::cout << Token::TypeToString(tokenType) << " BinaryOperationNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const PrefixUnaryOperationNode *node) override {
        indent += 2;

        const auto token = node->GetToken();
        const auto tokenType = token->GetType();

        PrintIndent();
        std::cout << Token::TypeToString(tokenType) << " PrefixUnaryOperationNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const InfiniteLoopNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "InfiniteLoopNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const PredicateLoopNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "PredicateLoopNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const IteratorLoopNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "IteratorLoopNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const IfNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "IfNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const BlockNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "BlockNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const BreakNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "BreakNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const ContinueNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "ContinueNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const ReturnNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "ReturnNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const CallOrInitTupleNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "CallOrInitTupleNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const IndexNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "IndexNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const MemberAccessNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "MemberAccessNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }
    void PostVisit(const ArrayExpressionNode *node) override {
        indent += 2;

        PrintIndent();
        if (node->IsSemiMode()) {
            std::cout << "; ";
        }
        std::cout << "ArrayExpressionNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const InitStructExpressionNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "InitStructExpressionNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const ShorthandFieldInitStructExpressionNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "ShorthandFieldInitStructExpressionNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const TupleIndexFieldInitStructExpressionNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "TupleIndexFieldInitStructExpressionNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const IdentifierFieldInitStructExpressionNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "IdentifierFieldInitStructExpressionNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const TupleExpressionNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "TupleExpressionNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const SyntaxTree *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "SyntaxTree" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const AssignmentNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << Token::TypeToString(node->GetOperation().GetType()) << " AssignmentNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }
};

int main(int argc, char **argv) {
    bool print_tokenizer = false;

    std::string filename = "main.txt";
    // bool filename_found = true;

    // for (int k = 1; k < argc; k++) {
    //    std::string arg(argv[k]);

    //    if (arg == "-t") {
    //        print_tokenizer = true;
    //    } else if (filename_found) {
    //        std::cerr << "invalid arguments" << std::endl;
    //        return 0;
    //    } else {
    //        filename = arg;
    //        filename_found = true;
    //    }
    //}

    // if (!filename_found) {
    //    return 0;
    //}

    std::ifstream ifs(filename);

    Tokenizer tokenizer(&ifs, Tokenizer::TargetType::kX64);
    if (print_tokenizer) {
        while (tokenizer.HasNext()) {
            Token token = tokenizer.Next();
            if (token.GetType() == Token::Type::kEndOfFile) {
                break;
            }

            std::cout << token.ToString(&ifs) << std::endl;
        }
    }

    SyntaxParser parser(&tokenizer);
    std::unique_ptr<SyntaxTree> syntax_tree = parser.ParseStatements();

    MyVisitor visitor;
    visitor.Visit(syntax_tree.get());

    ifs.close();

    return 0;
}
