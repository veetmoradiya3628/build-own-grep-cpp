#include "Matcher.h"
#include <functional> // Required for continuations
#include <vector>

static bool match_nodes(
    const std::vector<std::unique_ptr<RegexNode>> &nodes,
    size_t node_idx,
    std::string_view text,
    const MatchContext &context,
    const std::function<bool(std::string_view)> &continuation)
{
    // Base case: we finished all nodes in this sequence. Now try the continuation.
    if (node_idx == nodes.size())
    {
        return continuation(text);
    }

    const auto &node = nodes[node_idx];

    if (node->is_alternation())
    {
        const AlternationNode *alt = static_cast<const AlternationNode *>(node.get());

        auto next_continuation = [&](std::string_view remaining_text)
        {
            return match_nodes(nodes, node_idx + 1, remaining_text, context, continuation);
        };

        for (const auto &branch : alt->branches)
        {
            if (match_nodes(branch, 0, text, context, next_continuation))
            {
                return true; // Match found!
            }
        }
        return false; // All branches failed, trigger backtrack
    }

    if (node->quantifier == Quantifier::OneOrMore)
    {
        int total_consumed = 0;
        std::string_view current_text = text;
        std::vector<int> match_lengths;

        while (true)
        {
            int consumed = node->match(current_text);
            if (consumed == -1 || consumed == 0)
                break;

            match_lengths.push_back(consumed);
            total_consumed += consumed;
            current_text.remove_prefix(consumed);
        }

        if (match_lengths.empty())
            return false;

        while (!match_lengths.empty())
        {
            if (match_nodes(nodes, node_idx + 1, text.substr(total_consumed), context, continuation))
            {
                return true;
            }
            total_consumed -= match_lengths.back();
            match_lengths.pop_back();
        }
        return false;
    }
    else if (node->quantifier == Quantifier::ZeroOrOne)
    {
        int consumed = node->match(text);
        if (consumed != -1)
        {
            if (match_nodes(nodes, node_idx + 1, text.substr(consumed), context, continuation))
            {
                return true;
            }
        }
        return match_nodes(nodes, node_idx + 1, text, context, continuation);
    }
    else
    {
        // Standard single match
        int consumed = node->match(text);
        if (consumed != -1)
        {
            text.remove_prefix(consumed);
            return match_nodes(nodes, node_idx + 1, text, context, continuation);
        }
        return false;
    }
}

bool is_match(const ParsedRegex &regex, std::string_view text)
{
    auto final_continuation = [&](std::string_view remaining_text)
    {
        if (regex.context.has_end_anchor)
        {
            return remaining_text.empty();
        }
        return true;
    };

    if (regex.context.has_start_anchor)
    {
        return match_nodes(regex.nodes, 0, text, regex.context, final_continuation);
    }

    for (size_t i = 0; i <= text.length(); ++i)
    {
        if (match_nodes(regex.nodes, 0, text.substr(i), regex.context, final_continuation))
        {
            return true;
        }
    }
    return false;
}