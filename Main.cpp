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
        std::cout << "ParamFunctionNode" << std::endl;

        Visit(paramFunctionNode->GetPattern());
        Visit(paramFunctionNode->GetType());

        indent -= 2;
    }

    void PostVisit(const ParamStructNode *paramStructNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "ParamStructNode" << std::endl;

        Visit(paramStructNode->GetIdentifier());
        Visit(paramStructNode->GetType());

        indent -= 2;
    }

    void PostVisit(const LetNode *letNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "LetNode" << std::endl;

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
        std::cout << "FunctionNode" << std::endl;

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
        std::cout << "StructNode" << std::endl;

        for (const ParamStructNode *param : structNode->GetParams()) {
            Visit(param);
        }

        indent -= 2;
    }

    void PostVisit(const ConstantItemNode *constantItemNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "ConstantItemNode" << std::endl;

        Visit(constantItemNode->GetIdentifier());
        Visit(constantItemNode->GetType());
        Visit(constantItemNode->GetExpr());

        indent -= 2;
    }

    void PostVisit(const ParenthesizedTypeNode *parenthesizedTypeNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "ParenthesizedTypeNode" << std::endl;

        Visit(parenthesizedTypeNode->GetType());

        indent -= 2;
    }

    void PostVisit(const TupleTypeNode *tupleTypeNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "TupleTypeNode" << std::endl;

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
        std::cout << "ReferenceTypeNode" << std::endl;

        Visit(referenceTypeNode->GetType());

        indent -= 2;
    }

    void PostVisit(const ArrayTypeNode *arrayTypeNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "ArrayTypeNode" << std::endl;

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
        std::cout << "TupleIndexFieldNode" << std::endl;

        Visit(tupleIndexFieldNode->GetLiteral());
        Visit(tupleIndexFieldNode->GetPattern());

        indent -= 2;
    }

    void PostVisit(const IdentifierFieldNode *identifierFieldNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "IdentifierFieldNode" << std::endl;

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
        std::cout << "RefMutIdentifierFieldNode" << std::endl;

        Visit(refMutIdentifierFieldNode->GetIdentifier());

        indent -= 2;
    }

    void PostVisit(const LiteralPatternNode *literalPatternNode) override {
        Visit(literalPatternNode->GetLiteral());
    }

    void PostVisit(const IdentifierPatternNode *identifierPatternNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "IdentifierPatternNode" << std::endl;

        Visit(identifierPatternNode->GetIdentifier());
        Visit(identifierPatternNode->GetPattern());

        indent -= 2;
    }

    void PostVisit(const WildcardPatternNode *wildcardPatternNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "WildcardPatternNode" << std::endl;

        indent -= 2;
    }

    void PostVisit(const RestPatternNode *restPatternNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "RestPatternNode" << std::endl;

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
        std::cout << "ReferencePatternNode" << std::endl;

        Visit(referencePatternNode->GetPattern());

        indent -= 2;
    }

    void PostVisit(const StructPatternNode *structPatternNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "StructPatternNode" << std::endl;

        Visit(structPatternNode->GetIdentifier());
        for (const FieldNode *field : structPatternNode->GetFields()) {
            Visit(field);
        }

        indent -= 2;
    }

    void PostVisit(const TupleStructPatternNode *tupleStructPatternNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "TupleStructPatternNode" << std::endl;

        Visit(tupleStructPatternNode->GetIdentifier());
        for (const PatternNode *pattern : tupleStructPatternNode->GetPatterns()) {
            Visit(pattern);
        }

        indent -= 2;
    }

    void PostVisit(const TuplePatternNode *tuplePatternNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "TuplePatternNode" << std::endl;

        for (const PatternNode *pattern : tuplePatternNode->GetPatterns()) {
            Visit(pattern);
        }

        indent -= 2;
    }

    void PostVisit(const GroupedPatternNode *groupedPatternNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "GroupedPatternNode" << std::endl;

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
        std::cout << Token::TypeToString(tokenType) << " BinaryOperationNode" << std::endl;

        Visit(binaryOperationNode->GetLeft());
        Visit(binaryOperationNode->GetRight());

        indent -= 2;
    }

    void PostVisit(const PrefixUnaryOperationNode *prefixUnaryOperationNode) override {
        indent += 2;

        const auto token = prefixUnaryOperationNode->GetToken();
        const auto tokenType = token->GetType();

        PrintIndent();
        std::cout << Token::TypeToString(tokenType) << " PrefixUnaryOperationNode" << std::endl;

        Visit(prefixUnaryOperationNode->GetRight());

        indent -= 2;
    }

    void PostVisit(const InfiniteLoopNode *infiniteLoopNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "InfiniteLoopNode" << std::endl;

        Visit(infiniteLoopNode->GetBlock());

        indent -= 2;
    }

    void PostVisit(const PredicateLoopNode *predicateLoopNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "PredicateLoopNode" << std::endl;

        Visit(predicateLoopNode->GetExpression());
        Visit(predicateLoopNode->GetBlock());

        indent -= 2;
    }

    void PostVisit(const IteratorLoopNode *iteratorLoopNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "IteratorLoopNode" << std::endl;

        Visit(iteratorLoopNode->GetPattern());
        Visit(iteratorLoopNode->GetExpression());
        Visit(iteratorLoopNode->GetBlock());

        indent -= 2;
    }

    void PostVisit(const IfNode *ifNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "IfNode" << std::endl;

        Visit(ifNode->GetExpression());
        Visit(ifNode->GetIfBlock());
        Visit(ifNode->GetElseBlock());
        Visit(ifNode->GetElseIf());

        indent -= 2;
    }

    void PostVisit(const BlockNode *blockNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "BlockNode" << std::endl;

        for (const SyntaxNode *statement : blockNode->GetStatements()) {
            Visit(statement);
        }

        Visit(blockNode->GetReturnExpression());

        indent -= 2;
    }

    void PostVisit(const BreakNode *breakNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "BreakNode" << std::endl;

        Visit(breakNode->GetExpression());

        indent -= 2;
    }

    void PostVisit(const ContinueNode *continueNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "ContinueNode" << std::endl;

        indent -= 2;
    }

    void PostVisit(const ReturnNode *returnNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "ReturnNode" << std::endl;

        Visit(returnNode->GetExpression());

        indent -= 2;
    }

    void PostVisit(const CallOrInitTupleNode *callNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "CallOrInitTupleNode" << std::endl;

        Visit(callNode->GetIdentifier());
        for (const ExpressionNode *argument : callNode->GetArguments()) {
            Visit(argument);
        }

        indent -= 2;
    }

    void PostVisit(const IndexNode *indexNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "IndexNode" << std::endl;

        Visit(indexNode->GetIdentifier());
        Visit(indexNode->GetExpression());

        indent -= 2;
    }

    void PostVisit(const MemberAccessNode *memberAccessNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "MemberAccessNode" << std::endl;

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
        std::cout << "ArrayExpressionNode" << std::endl;
        for (const ExpressionNode *expreesion : arrayExpressionNode->GetExpressions()) {
            Visit(expreesion);
        }

        indent -= 2;
    }

    void PostVisit(const InitStructExpressionNode *initStructExpressionNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "InitStructExpressionNode" << std::endl;

        Visit(initStructExpressionNode->GetIdentifier());
        Visit(initStructExpressionNode->GetDotDotExpression());
        for (const FieldInitStructExpressionNode *field : initStructExpressionNode->GetFields()) {
            Visit(field);
        }

        indent -= 2;
    }

    void PostVisit(const ShorthandFieldInitStructExpressionNode *shorthandFieldInitStructExpressionNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "ShorthandFieldInitStructExpressionNode" << std::endl;

        Visit(shorthandFieldInitStructExpressionNode->GetIdentifier());

        indent -= 2;
    }

    void PostVisit(const TupleIndexFieldInitStructExpressionNode *tupleIndexFieldInitStructExpressionNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "TupleIndexFieldInitStructExpressionNode" << std::endl;

        Visit(tupleIndexFieldInitStructExpressionNode->GetLiteral());
        Visit(tupleIndexFieldInitStructExpressionNode->GetExpression());

        indent -= 2;
    }

    void PostVisit(const IdentifierFieldInitStructExpressionNode *identifierFieldInitStructExpressionNode) override {
        indent += 2;

        PrintIndent();
        std::cout << "IdentifierFieldInitStructExpressionNode" << std::endl;

        Visit(identifierFieldInitStructExpressionNode->GetIdentifier());
        Visit(identifierFieldInitStructExpressionNode->GetExpression());

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
