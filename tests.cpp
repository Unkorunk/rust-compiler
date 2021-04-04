#include <gtest/gtest.h>
#include <string>
#include <fstream>
#include <sstream>

#include "Tokenizer.hpp"

void my_test(const std::string& test_suit_name, const std::string& test_name) {
    std::ifstream ifs("tests/" + test_suit_name + "/" + test_name + "/input.txt");
    
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

    ifs.open("tests/"+ test_suit_name + "/" + test_name + "/correct.txt");
    std::string output((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    ifs.close();

    std::ofstream ofs("tests/"+ test_suit_name + "/" + test_name + "/output.txt");
    ofs << input;
    ofs.close();

    ASSERT_STREQ(input.c_str(), output.c_str());
}

#define MY_TEST(test_suit_name, test_name, path, folder) \
    TEST(test_suit_name, test_name)                      \
    {                                                    \
        my_test(path, folder);                           \
    }

MY_TEST(WhitespaceTest, Test1, "whitespace", "test1")
MY_TEST(WhitespaceTest, Test2, "whitespace", "test2")
MY_TEST(WhitespaceTest, Test3, "whitespace", "test3")

MY_TEST(PunctuationTest, Test1, "punctuation", "test1")
MY_TEST(PunctuationTest, Test2, "punctuation", "test2 (error)")
MY_TEST(PunctuationTest, Test3, "punctuation", "test3")
MY_TEST(PunctuationTest, Test4, "punctuation", "test4")
MY_TEST(PunctuationTest, Test5, "punctuation", "test5")
MY_TEST(PunctuationTest, Test6, "punctuation", "test6")

MY_TEST(LiteralCharacterTest, Test1, "literals/character", "test1")
MY_TEST(LiteralCharacterTest, Test2, "literals/character", "test2")
MY_TEST(LiteralCharacterTest, Test3, "literals/character", "test3 (error)")
MY_TEST(LiteralCharacterTest, Test4, "literals/character", "test4")

MY_TEST(LiteralStringTest, Test1, "literals/string", "test1")
MY_TEST(LiteralStringTest, Test2, "literals/string", "test2")
MY_TEST(LiteralStringTest, Test3, "literals/string", "test3 (error)")
MY_TEST(LiteralStringTest, Test4, "literals/string", "test4")
MY_TEST(LiteralStringTest, Test5, "literals/string", "test5")

MY_TEST(LiteralRawStringTest, Test1, "literals/raw string", "test1")
MY_TEST(LiteralRawStringTest, Test2, "literals/raw string", "test2")
MY_TEST(LiteralRawStringTest, Test3, "literals/raw string", "test3")
MY_TEST(LiteralRawStringTest, Test4, "literals/raw string", "test4 (error)")

MY_TEST(LiteralByteTest, Test1, "literals/byte", "test1")
MY_TEST(LiteralByteTest, Test2, "literals/byte", "test2")
MY_TEST(LiteralByteTest, Test3, "literals/byte", "test3")
MY_TEST(LiteralByteTest, Test4, "literals/byte", "test4")

MY_TEST(LiteralByteStringTest, Test1, "literals/byte string", "test1")
MY_TEST(LiteralByteStringTest, Test2, "literals/byte string", "test2")
MY_TEST(LiteralByteStringTest, Test3, "literals/byte string", "test3")
MY_TEST(LiteralByteStringTest, Test4, "literals/byte string", "test4")
MY_TEST(LiteralByteStringTest, Test5, "literals/byte string", "test5")

MY_TEST(LiteralRawByteStringTest, Test1, "literals/raw byte string", "test1")
MY_TEST(LiteralRawByteStringTest, Test2, "literals/raw byte string", "test2")
MY_TEST(LiteralRawByteStringTest, Test3, "literals/raw byte string", "test3")
MY_TEST(LiteralRawByteStringTest, Test4, "literals/raw byte string", "test4 (error)")

MY_TEST(LiteralIntegerTest, Test1, "literals/integer", "test1")
MY_TEST(LiteralIntegerTest, Test2, "literals/integer", "test2 (error)")
MY_TEST(LiteralIntegerTest, Test3, "literals/integer", "test3 (error)")
MY_TEST(LiteralIntegerTest, Test4, "literals/integer", "test4")
MY_TEST(LiteralIntegerTest, Test5, "literals/integer", "test5 (complex)")
MY_TEST(LiteralIntegerTest, Test6, "literals/integer", "test6")

MY_TEST(LiteralFloatTest, Test1, "literals/float", "test1")
MY_TEST(LiteralFloatTest, Test2, "literals/float", "test2")
MY_TEST(LiteralFloatTest, Test3, "literals/float", "test3")
MY_TEST(LiteralFloatTest, Test4, "literals/float", "test4")
MY_TEST(LiteralFloatTest, Test5, "literals/float", "test5 (error)")

MY_TEST(LiteralBooleanTest, Test1, "literals/boolean", "test1")
MY_TEST(LiteralBooleanTest, Test2, "literals/boolean", "test2 (error)")

MY_TEST(KeywordsTest, Test1, "keywords", "test1")
MY_TEST(KeywordsTest, Test2, "keywords", "test2 (error)")
MY_TEST(KeywordsTest, Test3, "keywords", "test3 (complex)")

MY_TEST(CommentsTest, Test1, "comments", "test1")
MY_TEST(CommentsTest, Test2, "comments", "test2")
MY_TEST(CommentsTest, Test3, "comments", "test3")
MY_TEST(CommentsTest, Test4, "comments", "test4")

MY_TEST(IdentifiersTest, Test1, "identifiers", "test1")
MY_TEST(IdentifiersTest, Test2, "identifiers", "test2")
MY_TEST(IdentifiersTest, Test3, "identifiers", "test3 (error)")
MY_TEST(IdentifiersTest, Test4, "identifiers", "test4")
MY_TEST(IdentifiersTest, Test5, "identifiers", "test5 (error)")
