#include <iostream>
#include <string>
#include <stdexcept>
#include "Parser.h"
#include "Matcher.h"

int main(int argc, char *argv[])
{
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    bool only_matching = false;
    std::string pattern;

    if (argc == 4 && std::string(argv[1]) == "-o" && std::string(argv[2]) == "-E")
    {
        only_matching = true;
        pattern = argv[3];
    }
    else if (argc == 3 && std::string(argv[1]) == "-E")
    {
        pattern = argv[2];
    }
    else
    {
        std::cerr << "Usage: ./your_program.sh [-o] -E <pattern>" << std::endl;
        return 1;
    }

    try
    {
        ParsedRegex regex = parse_pattern(pattern);

        std::string input_line;
        bool found_any_match = false;

        while (std::getline(std::cin, input_line))
        {
            std::string_view match;

            if (only_matching)
            {
                std::string_view remaining = input_line;
                bool line_had_match = false;

                while (!remaining.empty() && find_match(regex, remaining, match))
                {
                    std::cout << match << std::endl;
                    line_had_match = true;

                    size_t match_start_idx = match.data() - remaining.data();
                    size_t characters_to_advance = (match.length() == 0) ? 1 : match.length();
                    remaining = remaining.substr(match_start_idx + characters_to_advance);
                }

                if (line_had_match)
                {
                    found_any_match = true;
                }
            }
            else
            {
                if (find_match(regex, input_line, match))
                {
                    std::cout << input_line << std::endl;
                    found_any_match = true;
                }
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