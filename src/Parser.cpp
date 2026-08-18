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

        if (c == '$' && i == pattern.length() - 1)
        {
            regex.context.has_end_anchor = true;
            std::cerr << "[DEBUG] Parsed End Anchor: $" << std::endl;
            continue; // Skip creating a node
        }

        if (c == '+')
        {
            if (!regex.nodes.empty())
            {
                regex.nodes.back()->quantifier = Quantifier::OneOrMore;
                std::cerr << "[DEBUG] Applied '+' quantifier to previous node" << std::endl;
            }
            else
            {
                std::cerr << "[DEBUG] Error: '+' with no preceding character" << std::endl;
            }
            continue;
        }

        if (c == '?')
        {
            if (!regex.nodes.empty())
            {
                regex.nodes.back()->quantifier = Quantifier::ZeroOrOne;
                std::cerr << "[DEBUG] Applied '?' quantifier to previous node" << std::endl;
            }
            else
            {
                std::cerr << "[DEBUG] Error: '?' with no preceding character" << std::endl;
            }
            continue;
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
        else if (c == '(')
        {
            std::string inner = "";
            i++;
            int paren_count = 1;
            while (i < pattern.length() && paren_count > 0)
            {
                if (pattern[i] == '(')
                    paren_count++;
                else if (pattern[i] == ')')
                    paren_count--;

                if (paren_count > 0)
                    inner += pattern[i];
                i++;
            }
            i--; // Adjust index because the main for-loop will increment it

            std::vector<std::string> branch_strs;
            std::string current_branch = "";
            int nested_parens = 0;

            for (char bc : inner)
            {
                if (bc == '(')
                    nested_parens++;
                else if (bc == ')')
                    nested_parens--;

                if (bc == '|' && nested_parens == 0)
                {
                    branch_strs.push_back(current_branch);
                    current_branch = "";
                }
                else
                {
                    current_branch += bc;
                }
            }
            branch_strs.push_back(current_branch);

            if (branch_strs.size() > 1)
            {
                std::vector<std::vector<std::unique_ptr<RegexNode>>> compiled_branches;
                for (const auto &b_str : branch_strs)
                {
                    compiled_branches.push_back(std::move(parse_pattern(b_str).nodes)); // Recursive compiling!
                }
                regex.nodes.push_back(std::make_unique<AlternationNode>(std::move(compiled_branches)));
                std::cerr << "[DEBUG] Parsed Alternation with " << branch_strs.size() << " branches" << std::endl;
            }
            else
            {
                ParsedRegex inner_regex = parse_pattern(inner);
                for (auto &n : inner_regex.nodes)
                {
                    regex.nodes.push_back(std::move(n));
                }
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
        else if (c == '.')
        {
            std::cerr << "[DEBUG] Parsed Wildcard: ." << std::endl;
            regex.nodes.push_back(std::make_unique<WildcardNode>());
        }
        else
        {
            regex.nodes.push_back(std::make_unique<LiteralNode>(c));
        }
    }

    return regex;
}