#pragma once
#include <memory>
#include <vector>
#include <string_view>
#include "RegexNode.h"
#include "MatchContext.h"

struct ParsedRegex
{
    std::vector<std::unique_ptr<RegexNode>> nodes;
    MatchContext context;
};

// Converts the raw string pattern into an AST
ParsedRegex parse_pattern(std::string_view pattern);