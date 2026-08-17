#include "Parser.h"
#include <iostream>

ParsedRegex parse_pattern(std::string_view pattern)
{
    ParsedRegex regex;

    for (size_t i = 0; i < pattern.length(); ++i)
    {
        char c = pattern[i];

        if (c == '^' && i == 0)
        {
            regex.context.has_start_anchor = true;
            std::cerr << "[DEBUG] Parsed Start Anchor: ^" << std::endl;
            continue; // Move to the next character without making a node
        }

        if (c == '\\')
        {
            if (i + 1 < pattern.length())
            {
                char next_c = pattern[++i];
                if (next_c == 'd')
                {
                    regex.nodes.push_back(std::make_unique<DigitNode>());
                }
                else if (next_c == 'w')
                {
                    regex.nodes.push_back(std::make_unique<WordNode>());
                }
                else
                {
                    regex.nodes.push_back(std::make_unique<LiteralNode>(next_c));
                }
            }
            else
            {
                regex.nodes.push_back(std::make_unique<LiteralNode>('\\'));
            }
        }
        else if (c == '[')
        {

            std::string group_chars = "";
            if (pattern[i + 1] == '^')
            {
                i += 2; // skip ^ as well
                while (i < pattern.length() && pattern[i] != ']')
                {
                    group_chars += pattern[i];
                    i++;
                }

                std::cerr << "[DEBUG] Parsed Negative Group: [" << group_chars << "]" << std::endl;
                regex.nodes.push_back(std::make_unique<NegativeGroupNode>(group_chars));
            }
            else
            {
                i++; // skip only [
                while (i < pattern.length() && pattern[i] != ']')
                {
                    group_chars += pattern[i];
                    i++;
                }

                std::cerr << "[DEBUG] Parsed Positive Group: [" << group_chars << "]" << std::endl;
                regex.nodes.push_back(std::make_unique<PositiveGroupNode>(group_chars));
            }
        }
        else
        {
            regex.nodes.push_back(std::make_unique<LiteralNode>(c));
        }
    }

    return regex;
}