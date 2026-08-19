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
    bool use_color = false;
    std::string pattern;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "-o")
        {
            only_matching = true;
        }
        else if (arg == "--color=always")
        {
            use_color = true;
        }
        else if (arg == "-E" && i + 1 < argc)
        {
            pattern = argv[++i]; // Grab the pattern and increment 'i'
        }
    }

    if (pattern.empty())
    {
        std::cerr << "Usage: ./your_program.sh [--color=always] [-o] -E <pattern>" << std::endl;
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

            if (find_match(regex, input_line, match))
            {
                found_any_match = true;

                if (use_color)
                {
                    std::string_view remaining = input_line;

                    while (!remaining.empty() && find_match(regex, remaining, match))
                    {

                        size_t match_start_idx = match.data() - remaining.data();

                        std::cout << remaining.substr(0, match_start_idx);

                        if (match.length() > 0)
                        {
                            std::cout << "\033[01;31m" << match << "\033[m";
                        }

                        size_t characters_to_advance = match_start_idx + (match.length() == 0 ? 1 : match.length());
                        remaining = remaining.substr(characters_to_advance);
                    }

                    std::cout << remaining << std::endl;
                }
                else if (only_matching)
                {
                    std::string_view remaining = input_line;

                    while (!remaining.empty() && find_match(regex, remaining, match))
                    {
                        if (match.length() > 0)
                        {
                            std::cout << match << std::endl;
                        }

                        size_t match_start_idx = match.data() - remaining.data();
                        size_t characters_to_advance = match_start_idx + (match.length() == 0 ? 1 : match.length());
                        remaining = remaining.substr(characters_to_advance);
                    }
                }
                else
                {
                    // Standard grep mode: print the whole line unmodified
                    std::cout << input_line << std::endl;
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