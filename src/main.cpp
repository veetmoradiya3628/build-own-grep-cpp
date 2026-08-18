#include <iostream>
#include <string>
#include <stdexcept>
#include "Parser.h"
#include "Matcher.h"

int main(int argc, char *argv[])
{
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    if (argc != 3 || std::string(argv[1]) != "-E")
    {
        std::cerr << "Usage: ./your_program.sh -E <pattern>" << std::endl;
        return 1;
    }

    std::string pattern = argv[2];

    try
    {
        ParsedRegex regex = parse_pattern(pattern);

        std::string input_line;
        bool found_any_match = false;

        while (std::getline(std::cin, input_line))
        {

            if (is_match(regex, input_line))
            {
                std::cout << input_line << std::endl;
                found_any_match = true;
            }
        }

        return found_any_match ? 0 : 1;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Runtime Error: " << e.what() << std::endl;
        return 1;
    }
}