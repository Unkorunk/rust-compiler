#include <gtest/gtest.h>

#include <fstream>
#include <sstream>
#include <string>

#include "SyntaxParser.hpp"
#include "ISyntaxTreeVisitor.hpp"
#include "Tokenizer.hpp"

class MyVisitor : public ISyntaxTreeVisitor {
    std::ostringstream *out_;

public:
    explicit MyVisitor(std::ostringstream *out) : out_(out) {}

protected:
    int indent = 0;
    void PrintIndent() const {
        for (int i = 0; i < indent; i++) {
            *out_ << ' ';
        }
    }

    void PostVisit(const LiteralNode *literalNode) override {
        indent += 2;

        const auto token = literalNode->GetToken();
        const auto tokenValue = token->GetTokenValue();

        PrintIndent();
        *out_ << tokenValue.ToString() << '\n';

        indent -= 2;
    }

    void PostVisit(const IdentifierNode *identifierNode) override {
        indent += 2;

        const auto token = identifierNode->GetToken();
        const auto tokenValue = token->GetTokenValue();

        PrintIndent();
        *out_ << tokenValue.ToString() << '\n';

        indent -= 2;
    }

    void PostVisit(const PrefixUnaryOperationNode *node) override {
        indent += 2;

        const auto token = node->GetToken();
        const auto tokenType = token->GetType();

        PrintIndent();
        *out_ << Token::TypeToString(tokenType) << '\n';
        Visit(node->GetRight());

        indent -= 2;
    }

    void PostVisit(const BinaryOperationNode *binaryOperationNode) override {
        indent += 2;
        const auto token = binaryOperationNode->GetToken();
        const auto tokenType = token->GetType();

        PrintIndent();
        *out_ << Token::TypeToString(tokenType) << '\n';

        Visit(binaryOperationNode->GetLeft());
        Visit(binaryOperationNode->GetRight());

        indent -= 2;
    }

    void PostVisit(const ErrorNode *errorNode) override {
        indent++;

        PrintIndent();
        *out_ << errorNode->GetError() << " " << errorNode->GetPosition().start_line << " "
              << errorNode->GetPosition().start_column << '\n';

        indent--;
    }
};

void TestTokenizer(const std::string &test_suit_name, const std::string &test_name) {
    std::ifstream ifs("tests/" + test_suit_name + "/" + test_name + "/input.txt");

    ASSERT_TRUE(ifs.is_open());

    Tokenizer tokenizer(&ifs, Tokenizer::TargetType::kX64);
    std::ostringstream oss;
    while (tokenizer.HasNext()) {
        Token token = tokenizer.Next();
        if (token.GetType() == Token::Type::kEndOfFile) {
            break;
        }

        oss << token.ToString(&ifs) << std::endl;
    }

    std::string input = oss.str();

    ifs.close();

    ifs.open("tests/" + test_suit_name + "/" + test_name + "/correct.txt");
    std::string output((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    ifs.close();

    std::ofstream ofs("tests/" + test_suit_name + "/" + test_name + "/output.txt");
    ofs << input;
    ofs.close();

    ASSERT_STREQ(input.c_str(), output.c_str());
}

void TestParser(const std::string &test_suit_name, const std::string &test_name) {
    std::ifstream ifs("tests/" + test_suit_name + "/" + test_name + "/input.txt");

    ASSERT_TRUE(ifs.is_open());

    Tokenizer tokenizer(&ifs, Tokenizer::TargetType::kX64);
    std::ostringstream oss;
    SyntaxParser parser(&tokenizer);
    std::unique_ptr<ExpressionNode> expression = parser.ParseExpr();

    MyVisitor visitor(&oss);
    visitor.Visit(expression.get());

    std::string input = oss.str();
    input = input.substr(0, input.size() - 1);

    ifs.close();

    ifs.open("tests/" + test_suit_name + "/" + test_name + "/correct.txt");
    std::string output((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    ifs.close();

    std::ofstream ofs("tests/" + test_suit_name + "/" + test_name + "/output.txt");
    ofs << input;
    ofs.close();

    ASSERT_STREQ(input.c_str(), output.c_str());
}

#define TEST_TOKENIZER(test_suit_name, test_name, path, folder) \
    TEST(test_suit_name, test_name) {                    \
        TestTokenizer(path, folder);                     \
    }


#define TEST_PARSER(test_suit_name, test_name, path, folder) \
    TEST(test_suit_name, test_name) {                        \
        TestParser(path, folder);                            \
    }

TEST_PARSER(ExpressionTest, Test1, "expression", "test1")
TEST_PARSER(ExpressionTest, Test2, "expression", "test2")
TEST_PARSER(ExpressionTest, Test3, "expression", "test3")
TEST_PARSER(ExpressionTest, Test4, "expression", "test4")
TEST_PARSER(ExpressionTest, Test5, "expression", "test5")
TEST_PARSER(ExpressionTest, Test6, "expression", "test6 (error)")
TEST_PARSER(ExpressionTest, Test7, "expression", "test7 (error)")
TEST_PARSER(ExpressionTest, Test8, "expression", "test8")
TEST_PARSER(ExpressionTest, Test9, "expression", "test9 (error)")
TEST_PARSER(ExpressionTest, Test10, "expression", "test10 (error)")

TEST_TOKENIZER(WhitespaceTest, Test1, "whitespace", "test1")
TEST_TOKENIZER(WhitespaceTest, Test2, "whitespace", "test2")
TEST_TOKENIZER(WhitespaceTest, Test3, "whitespace", "test3")

TEST_TOKENIZER(PunctuationTest, Test1, "punctuation", "test1")
TEST_TOKENIZER(PunctuationTest, Test2, "punctuation", "test2 (error)")
TEST_TOKENIZER(PunctuationTest, Test3, "punctuation", "test3")
TEST_TOKENIZER(PunctuationTest, Test4, "punctuation", "test4")
TEST_TOKENIZER(PunctuationTest, Test5, "punctuation", "test5")
TEST_TOKENIZER(PunctuationTest, Test6, "punctuation", "test6")

TEST_TOKENIZER(LiteralCharacterTest, Test1, "literals/character", "test1")
TEST_TOKENIZER(LiteralCharacterTest, Test2, "literals/character", "test2")
TEST_TOKENIZER(LiteralCharacterTest, Test3, "literals/character", "test3 (error)")
TEST_TOKENIZER(LiteralCharacterTest, Test4, "literals/character", "test4")

TEST_TOKENIZER(LiteralStringTest, Test1, "literals/string", "test1")
TEST_TOKENIZER(LiteralStringTest, Test2, "literals/string", "test2")
TEST_TOKENIZER(LiteralStringTest, Test3, "literals/string", "test3 (error)")
TEST_TOKENIZER(LiteralStringTest, Test4, "literals/string", "test4")
TEST_TOKENIZER(LiteralStringTest, Test5, "literals/string", "test5")

TEST_TOKENIZER(LiteralRawStringTest, Test1, "literals/raw string", "test1")
TEST_TOKENIZER(LiteralRawStringTest, Test2, "literals/raw string", "test2")
TEST_TOKENIZER(LiteralRawStringTest, Test3, "literals/raw string", "test3")
TEST_TOKENIZER(LiteralRawStringTest, Test4, "literals/raw string", "test4 (error)")

TEST_TOKENIZER(LiteralByteTest, Test1, "literals/byte", "test1")
TEST_TOKENIZER(LiteralByteTest, Test2, "literals/byte", "test2")
TEST_TOKENIZER(LiteralByteTest, Test3, "literals/byte", "test3")
TEST_TOKENIZER(LiteralByteTest, Test4, "literals/byte", "test4")

TEST_TOKENIZER(LiteralByteStringTest, Test1, "literals/byte string", "test1")
TEST_TOKENIZER(LiteralByteStringTest, Test2, "literals/byte string", "test2")
TEST_TOKENIZER(LiteralByteStringTest, Test3, "literals/byte string", "test3")
TEST_TOKENIZER(LiteralByteStringTest, Test4, "literals/byte string", "test4")
TEST_TOKENIZER(LiteralByteStringTest, Test5, "literals/byte string", "test5")

TEST_TOKENIZER(LiteralRawByteStringTest, Test1, "literals/raw byte string", "test1")
TEST_TOKENIZER(LiteralRawByteStringTest, Test2, "literals/raw byte string", "test2")
TEST_TOKENIZER(LiteralRawByteStringTest, Test3, "literals/raw byte string", "test3")
TEST_TOKENIZER(LiteralRawByteStringTest, Test4, "literals/raw byte string", "test4 (error)")

TEST_TOKENIZER(LiteralIntegerTest, Test1, "literals/integer", "test1")
TEST_TOKENIZER(LiteralIntegerTest, Test2, "literals/integer", "test2 (error)")
TEST_TOKENIZER(LiteralIntegerTest, Test3, "literals/integer", "test3 (error)")
TEST_TOKENIZER(LiteralIntegerTest, Test4, "literals/integer", "test4")
TEST_TOKENIZER(LiteralIntegerTest, Test5, "literals/integer", "test5 (complex)")
TEST_TOKENIZER(LiteralIntegerTest, Test6, "literals/integer", "test6")

TEST_TOKENIZER(LiteralFloatTest, Test1, "literals/float", "test1")
TEST_TOKENIZER(LiteralFloatTest, Test2, "literals/float", "test2")
TEST_TOKENIZER(LiteralFloatTest, Test3, "literals/float", "test3")
TEST_TOKENIZER(LiteralFloatTest, Test4, "literals/float", "test4")
TEST_TOKENIZER(LiteralFloatTest, Test5, "literals/float", "test5 (error)")

TEST_TOKENIZER(LiteralBooleanTest, Test1, "literals/boolean", "test1")
TEST_TOKENIZER(LiteralBooleanTest, Test2, "literals/boolean", "test2 (error)")

TEST_TOKENIZER(KeywordsTest, Test1, "keywords", "test1")
TEST_TOKENIZER(KeywordsTest, Test2, "keywords", "test2 (error)")
TEST_TOKENIZER(KeywordsTest, Test3, "keywords", "test3 (complex)")

TEST_TOKENIZER(CommentsTest, Test1, "comments", "test1")
TEST_TOKENIZER(CommentsTest, Test2, "comments", "test2")
TEST_TOKENIZER(CommentsTest, Test3, "comments", "test3")
TEST_TOKENIZER(CommentsTest, Test4, "comments", "test4")

TEST_TOKENIZER(IdentifiersTest, Test1, "identifiers", "test1")
TEST_TOKENIZER(IdentifiersTest, Test2, "identifiers", "test2")
TEST_TOKENIZER(IdentifiersTest, Test3, "identifiers", "test3 (error)")
TEST_TOKENIZER(IdentifiersTest, Test4, "identifiers", "test4")
TEST_TOKENIZER(IdentifiersTest, Test5, "identifiers", "test5 (error)")
