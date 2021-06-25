#include <fstream>
#include <iostream>

#include "SyntaxParser.hpp"
#include "SyntaxTreeVisitor.hpp"
#include "Tokenizer.hpp"

class MyVisitor : public SyntaxTreeVisitor {
protected:
    int indent = 0;
    void PrintIndent() const {
        for (int i = 0; i < indent; i++) {
            std::cout << ' ';
        }
    }

    void PostVisit(const IdentifierNode *identifierNode) override {
        indent += 2;

        const auto token = identifierNode->GetToken();
        const auto tokenValue = token->GetTokenValue();

        PrintIndent();
        std::cout << tokenValue.ToString() << std::endl;

        indent -= 2;
    }

    void PostVisit(const LiteralNode *literalNode) override {
        indent += 2;

        const auto token = literalNode->GetToken();
        const auto tokenValue = token->GetTokenValue();

        PrintIndent();
        std::cout << tokenValue.ToString() << std::endl;

        indent -= 2;
    }

    void PostVisit(const ErrorNode *errorNode) override {
        indent++;

        PrintIndent();
        std::cout << errorNode->GetError() << " " << errorNode->GetPosition().start_line << " "
                  << errorNode->GetPosition().start_column << std::endl;

        indent--;
    }

    void PostVisit(const ParamFunctionNode *paramFunctionNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "param function" << std::endl;

        Visit(paramFunctionNode->GetPattern());
        Visit(paramFunctionNode->GetType());

        indent -= 2;
    }

    void PostVisit(const ParamStructNode *paramStructNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "param struct" << std::endl;

        Visit(paramStructNode->GetIdentifier());
        Visit(paramStructNode->GetType());

        indent -= 2;
    }

    void PostVisit(const LetNode *letNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "let" << std::endl;

        Visit(letNode->GetPattern());
        Visit(letNode->GetType());
        Visit(letNode->GetExpression());

        indent -= 2;
    }

    void PostVisit(const FunctionNode *functionNode) override {
        indent += 2;

        PrintIndent();
        if (functionNode->IsConst()) {
            std::cout << "const ";
        }
        std::cout << "function" << std::endl;

        Visit(functionNode->GetIdentifier());
        Visit(functionNode->GetReturnType());
        Visit(functionNode->GetBlock());

        for (const ParamFunctionNode *param : functionNode->GetParams()) {
            Visit(param);
        }

        indent -= 2;
    }

    void PostVisit(const StructNode *structNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "struct" << std::endl;

        for (const ParamStructNode *param : structNode->GetParams()) {
            Visit(param);
        }

        indent -= 2;
    }

    void PostVisit(const ConstantItemNode *constantItemNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "constant item" << std::endl;

        Visit(constantItemNode->GetIdentifier());
        Visit(constantItemNode->GetType());
        Visit(constantItemNode->GetExpr());

        indent -= 2;
    }

    void PostVisit(const ParenthesizedTypeNode *parenthesizedTypeNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "parenthesized type" << std::endl;

        Visit(parenthesizedTypeNode->GetType());

        indent -= 2;
    }

    void PostVisit(const TupleTypeNode *tupleTypeNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "tuple type" << std::endl;

        for (const TypeNode *type : tupleTypeNode->GetTypes()) {
            Visit(type);
        }

        indent -= 2;
    }

    void PostVisit(const ReferenceTypeNode *referenceTypeNode) override {
        indent += 2;

        PrintIndent();
        if (referenceTypeNode->IsMut()) {
            std::cout << "mut ";
        }
        std::cout << "reference type" << std::endl;

        Visit(referenceTypeNode->GetType());

        indent -= 2;
    }

    void PostVisit(const ArrayTypeNode *arrayTypeNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "array type" << std::endl;

        Visit(arrayTypeNode->GetType());
        Visit(arrayTypeNode->GetExpression());

        indent -= 2;
    }

    void PostVisit(const IdentifierTypeNode *identifierTypeNode) override {
        Visit(identifierTypeNode->GetIdentifier());
    }

    void PostVisit(const TupleIndexFieldNode *tupleIndexFieldNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "tuple index field" << std::endl;

        Visit(tupleIndexFieldNode->GetLiteral());
        Visit(tupleIndexFieldNode->GetPattern());

        indent -= 2;
    }

    void PostVisit(const IdentifierFieldNode *identifierFieldNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "identifier field" << std::endl;

        Visit(identifierFieldNode->GetIdentifier());
        Visit(identifierFieldNode->GetPattern());

        indent -= 2;
    }

    void PostVisit(const RefMutIdentifierFieldNode *refMutIdentifierFieldNode) override {
        indent += 2;

        PrintIndent();
        if (refMutIdentifierFieldNode->IsRef()) {
            std::cout << "ref ";
        }
        if (refMutIdentifierFieldNode->IsMut()) {
            std::cout << "mut ";
        }
        std::cout << "identifier field" << std::endl;

        Visit(refMutIdentifierFieldNode->GetIdentifier());

        indent -= 2;
    }

    void PostVisit(const LiteralPatternNode *literalPatternNode) override {
        Visit(literalPatternNode->GetLiteral());
    }

    void PostVisit(const IdentifierPatternNode *identifierPatternNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "identifier pattern" << std::endl;

        Visit(identifierPatternNode->GetIdentifier());
        Visit(identifierPatternNode->GetPattern());

        indent -= 2;
    }

    void PostVisit(const WildcardPatternNode *wildcardPatternNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "wildcard pattern" << std::endl;

        indent -= 2;
    }

    void PostVisit(const RestPatternNode *restPatternNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "rest pattern" << std::endl;

        indent -= 2;
    }

    void PostVisit(const ReferencePatternNode *referencePatternNode) override {
        indent += 2;

        PrintIndent();
        if (referencePatternNode->IsMut()) {
            std::cout << "mut ";
        }
        if (referencePatternNode->IsSingleRef()) {
            std::cout << "& ";
        } else {
            std::cout << "&& ";
        }
        std::cout << "reference pattern" << std::endl;

        Visit(referencePatternNode->GetPattern());

        indent -= 2;
    }

    void PostVisit(const StructPatternNode *structPatternNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "struct pattern" << std::endl;

        Visit(structPatternNode->GetIdentifier());
        for (const FieldNode *field : structPatternNode->GetFields()) {
            Visit(field);
        }

        indent -= 2;
    }

    void PostVisit(const TupleStructPatternNode *tupleStructPatternNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "tuple struct pattern" << std::endl;

        Visit(tupleStructPatternNode->GetIdentifier());
        for (const PatternNode *pattern : tupleStructPatternNode->GetPatterns()) {
            Visit(pattern);
        }

        indent -= 2;
    }

    void PostVisit(const TuplePatternNode *tuplePatternNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "tuple pattern" << std::endl;

        for (const PatternNode *pattern : tuplePatternNode->GetPatterns()) {
            Visit(pattern);
        }

        indent -= 2;
    }

    void PostVisit(const GroupedPatternNode *groupedPatternNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "grouped pattern" << std::endl;

        Visit(groupedPatternNode->GetPattern());

        indent -= 2;
    }

    void PostVisit(const IdentifierExpressionNode *identifierExpressionNode) override {
        Visit(identifierExpressionNode->GetIdentifier());
    }

    void PostVisit(const LiteralExpressionNode *literalExpressionNode) override {
        Visit(literalExpressionNode->GetLiteral());
    }

    void PostVisit(const BinaryOperationNode *binaryOperationNode) override {
        indent += 2;
        const auto token = binaryOperationNode->GetToken();
        const auto tokenType = token->GetType();

        PrintIndent();
        std::cout << Token::TypeToString(tokenType) << " binary operation" << std::endl;

        Visit(binaryOperationNode->GetLeft());
        Visit(binaryOperationNode->GetRight());

        indent -= 2;
    }

    void PostVisit(const PrefixUnaryOperationNode *prefixUnaryOperationNode) override {
        indent += 2;

        const auto token = prefixUnaryOperationNode->GetToken();
        const auto tokenType = token->GetType();

        PrintIndent();
        std::cout << Token::TypeToString(tokenType) << " prefix unary operation" << std::endl;

        Visit(prefixUnaryOperationNode->GetRight());

        indent -= 2;
    }

    void PostVisit(const InfiniteLoopNode *infiniteLoopNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "infinite loop" << std::endl;

        Visit(infiniteLoopNode->GetBlock());

        indent -= 2;
    }

    void PostVisit(const PredicateLoopNode *predicateLoopNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "predicate loop" << std::endl;

        Visit(predicateLoopNode->GetExpression());
        Visit(predicateLoopNode->GetBlock());

        indent -= 2;
    }

    void PostVisit(const IteratorLoopNode *iteratorLoopNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "iterator loop" << std::endl;

        Visit(iteratorLoopNode->GetPattern());
        Visit(iteratorLoopNode->GetExpression());
        Visit(iteratorLoopNode->GetBlock());

        indent -= 2;
    }

    void PostVisit(const IfNode *ifNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "if" << std::endl;

        Visit(ifNode->GetExpression());
        Visit(ifNode->GetIfBlock());
        Visit(ifNode->GetElseBlock());
        Visit(ifNode->GetElseIf());

        indent -= 2;
    }

    void PostVisit(const BlockNode *blockNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "block" << std::endl;

        for (const SyntaxNode *statement : blockNode->GetStatements()) {
            Visit(statement);
        }

        Visit(blockNode->GetReturnExpression());

        indent -= 2;
    }

    void PostVisit(const BreakNode *breakNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "break" << std::endl;

        Visit(breakNode->GetExpression());

        indent -= 2;
    }

    void PostVisit(const ContinueNode *continueNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "continue" << std::endl;

        indent -= 2;
    }

    void PostVisit(const ReturnNode *returnNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "return" << std::endl;

        Visit(returnNode->GetExpression());

        indent -= 2;
    }

    void PostVisit(const CallNode *callNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "call" << std::endl;

        Visit(callNode->GetIdentifier());
        for (const ExpressionNode *argument : callNode->GetArguments()) {
            Visit(argument);
        }

        indent -= 2;
    }

    void PostVisit(const IndexNode *indexNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "index" << std::endl;

        Visit(indexNode->GetIdentifier());
        Visit(indexNode->GetExpression());

        indent -= 2;
    }

    void PostVisit(const MemberAccessNode *memberAccessNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "member access" << std::endl;

        Visit(memberAccessNode->GetIdentifier());
        Visit(memberAccessNode->GetExpression());

        indent -= 2;
    }
    void PostVisit(const ArrayExpressionNode *arrayExpressionNode) override {
        indent += 2;

        PrintIndent();
        if (arrayExpressionNode->IsSemiMode()) {
            std::cout << "; ";
        }
        std::cout << "array expression" << std::endl;
        for (const ExpressionNode *expreesion : arrayExpressionNode->GetExpressions()) {
            Visit(expreesion);
        }

        indent -= 2;
    }
};

int main(int argc, char **argv) {
    bool print_tokenizer = false;

    std::string filename = "main.txt";
    bool filename_found = true;

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
    std::vector<std::unique_ptr<SyntaxNode>> statements = parser.ParseStatements();

    MyVisitor visitor;
    for (const auto &statement : statements) {
        visitor.Visit(statement.get());
    }

    ifs.close();

    return 0;
}
