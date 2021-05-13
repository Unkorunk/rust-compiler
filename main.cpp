#include <fstream>
#include <iostream>

#include "Tokenizer.hpp"

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

    ifs.close();

    return 0;
}
