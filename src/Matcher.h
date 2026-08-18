#pragma once
#include "Parser.h"
#include <string_view>

// Returns true if a match is found, and populates out_match with the exact substring
bool find_match(const ParsedRegex& regex, std::string_view text, std::string_view& out_match);