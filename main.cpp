#include <iostream>
#include <fstream>

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

    if (!filename_found) return 0;

    std::basic_ifstream<Tokenizer::char_type, std::char_traits<Tokenizer::char_type>> ifs(filename.c_str());
    if (!ifs.is_open()) {
        std::cerr << "file open error" << std::endl;
        return 0;
    }

    Tokenizer tokenizer(&ifs);

    if (print_tokenizer) {
        while (tokenizer.HasNext()) {
            std::cout << tokenizer.Next().ToString() << std::endl;
        }

        return 0;
    }

    ifs.close();

    return 0;
}
