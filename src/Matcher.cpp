#include "Matcher.h"
#include <vector>

static bool match_nodes(const ParsedRegex &regex, size_t node_idx, std::string_view text)
{
    if (node_idx == regex.nodes.size())
    {
        if (regex.context.has_end_anchor)
        {
            return text.empty();
        }
        return true;
    }

    const auto &node = regex.nodes[node_idx];

    if (node->quantifier == Quantifier::OneOrMore)
    {
        int total_consumed = 0;
        std::string_view current_text = text;

        std::vector<int> match_lengths;

        while (true)
        {
            int consumed = node->match(current_text);
            if (consumed == -1 || consumed == 0)
                break; // Stop when the node no longer matches

            match_lengths.push_back(consumed);
            total_consumed += consumed;
            current_text.remove_prefix(consumed);
        }

        if (match_lengths.empty())
        {
            return false;
        }

        while (!match_lengths.empty())
        {
            if (match_nodes(regex, node_idx + 1, text.substr(total_consumed)))
            {
                return true; // We successfully finished the regex!
            }

            total_consumed -= match_lengths.back();
            match_lengths.pop_back();
        }

        return false;
    }
    else
    {
        // Standard single match logic for nodes with Quantifier::None
        int consumed = node->match(text);
        if (consumed != -1)
        {
            text.remove_prefix(consumed);
            return match_nodes(regex, node_idx + 1, text);
        }
        return false;
    }
}

bool is_match(const ParsedRegex &regex, std::string_view text)
{
    if (regex.context.has_start_anchor)
    {
        return match_nodes(regex, 0, text);
    }

    for (size_t i = 0; i <= text.length(); ++i)
    {
        if (match_nodes(regex, 0, text.substr(i)))
        {
            return true;
        }
    }
    return false;
}