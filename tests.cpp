#include <gtest/gtest.h>
#include <string>
#include <fstream>
#include <sstream>

#include "Tokenizer.hpp"

void my_test(const std::string& test_suit_name, const std::string& test_name) {
    std::ifstream ifs("tests/" + test_suit_name + "/" + test_name + "/input.txt");
    
    Tokenizer tokenizer(&ifs);
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

    ifs.open("tests/"+ test_suit_name + "/" + test_name + "/correct.txt");
    std::string output((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    ifs.close();

    std::ofstream ofs("tests/"+ test_suit_name + "/" + test_name + "/output.txt");
    ofs << input;
    ofs.close();

    ASSERT_STREQ(input.c_str(), output.c_str());
}

TEST(WhitespaceTest, Test1)
{
    my_test("whitespace", "test1");
}

TEST(WhitespaceTest, Test2)
{
    my_test("whitespace", "test2");
}

TEST(PunctuationTest, Test2)
{
    my_test("punctuation", "test1");
}

TEST(LiteralByteTest, Test1)
{
    my_test("literals/byte", "test1");
}

TEST(LiteralByteTest, Test2)
{
    my_test("literals/byte", "test2");
}

TEST(LiteralByteTest, Test3)
{
    my_test("literals/byte", "test3");
}

TEST(LiteralByteTest, Test4)
{
    my_test("literals/byte string", "test4");
}

TEST(LiteralByteStringTest, Test1)
{
    my_test("literals/byte string", "test1");
}

TEST(LiteralByteStringTest, Test2)
{
    my_test("literals/byte string", "test2");
}

TEST(LiteralByteStringTest, Test3)
{
    my_test("literals/byte string", "test3");
}

TEST(LiteralByteStringTest, Test4)
{
    my_test("literals/byte string", "test4");
}


TEST(LiteralByteStringTest, Test5)
{
    my_test("literals/byte string", "test5");
}