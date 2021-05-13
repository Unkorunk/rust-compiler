#include <fstream>
#include <iostream>

#include "ExpressionParser.hpp"
#include "SyntaxTreeVisitor.hpp"
#include "Tokenizer.hpp"

class MyVisitor : public SyntaxTreeVisitor {
protected:
    void PostVisit(const LiteralNode *literalNode) override {
        const auto token = literalNode->GetToken();
        const auto tokenValue = token->GetTokenValue();

        std::cout << tokenValue.ValueToString();
    }

    void PostVisit(const IdentifierNode *identifierNode) override {
        const auto token = identifierNode->GetToken();
        const auto tokenValue = token->GetTokenValue();

        std::cout << tokenValue.ValueToString();
    }

    void PostVisit(const PrefixUnaryOperationNode *prefixUnaryOperationNode) override {
        const auto token = prefixUnaryOperationNode->GetToken();
        const auto tokenType = token->GetType();

        std::cout << Token::TypeToString(tokenType);
        prefixUnaryOperationNode->GetRight()->Visit(this);
    }

    void PostVisit(const BinaryOperationNode *binaryOperationNode) override {
        const auto token = binaryOperationNode->GetToken();
        const auto tokenType = token->GetType();

        std::cout << "(";
        binaryOperationNode->GetLeft()->Visit(this);
        std::cout << ") " << Token::TypeToString(tokenType) << " (";
        binaryOperationNode->GetRight()->Visit(this);
        std::cout << ")";
    }
};

int main(int argc, char **argv) {
    bool print_tokenizer = false;

    std::string filename;
    bool filename_found = false;

    for (int k = 1; k < argc; k++) {
        std::string arg(argv[k]);

        if (arg == "-t") {
            print_tokenizer = true;
        } else if (filename_found) {
            std::cerr << "invalid arguments" << std::endl;
            return 0;
        } else {
            filename = arg;
            filename_found = true;
        }
    }

    if (!filename_found) {
        return 0;
    }

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

    ExpressionParser parser(&tokenizer);
    auto syntaxTree = parser.Parse();

    MyVisitor visitor;
    visitor.Visit(syntaxTree.get());

    ifs.close();

    return 0;
}
