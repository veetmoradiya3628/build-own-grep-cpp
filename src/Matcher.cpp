#include "Matcher.h"

// Helper function to recursively traverse the AST
static bool match_nodes(const ParsedRegex &regex, size_t node_idx, std::string_view text)
{
    // Base case
    if (node_idx == regex.nodes.size())
    {
        if (regex.context.has_end_anchor)
        {
            return text.empty();
        }
        return true;
    }

    // Try to match the current node against the start of the text
    int consumed = regex.nodes[node_idx]->match(text);

    if (consumed != -1)
    {
        text.remove_prefix(consumed);
        return match_nodes(regex, node_idx + 1, text);
    }

    return false; // Match failed
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