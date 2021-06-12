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

    void PostVisit(const LiteralNode *literalNode) override {
        indent += 2;

        const auto token = literalNode->GetToken();
        const auto tokenValue = token->GetTokenValue();

        PrintIndent();
        std::cout << tokenValue.ToString() << std::endl;

        indent -= 2;
    }

    void PostVisit(const IdentifierNode *identifierNode) override {
        indent += 2;

        const auto token = identifierNode->GetToken();
        const auto tokenValue = token->GetTokenValue();

        PrintIndent();
        std::cout << tokenValue.ToString() << std::endl;

        indent -= 2;
    }

    void PostVisit(const PrefixUnaryOperationNode *prefixUnaryOperationNode) override {
        indent += 2;

        const auto token = prefixUnaryOperationNode->GetToken();
        const auto tokenType = token->GetType();

        PrintIndent();
        std::cout << Token::TypeToString(tokenType) << std::endl;
        Visit(prefixUnaryOperationNode->GetRight());

        indent -= 2;
    }

    void PostVisit(const BinaryOperationNode *binaryOperationNode) override {
        indent += 2;
        const auto token = binaryOperationNode->GetToken();
        const auto tokenType = token->GetType();

        PrintIndent();
        std::cout << Token::TypeToString(tokenType) << std::endl;

        Visit(binaryOperationNode->GetLeft());
        Visit(binaryOperationNode->GetRight());

        indent -= 2;
    }

    void PostVisit(const ErrorNode *errorNode) override {
        indent++;

        PrintIndent();
        std::cout << errorNode->GetError() << " " << errorNode->GetPosition().start_line << " "
                  << errorNode->GetPosition().start_column << std::endl;

        indent--;
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

    SyntaxParser parser(&tokenizer);
    std::unique_ptr<ExpressionNode> expression = parser.ParseExpr();

    MyVisitor visitor;
    visitor.Visit(expression.get());

    ifs.close();

    return 0;
}
