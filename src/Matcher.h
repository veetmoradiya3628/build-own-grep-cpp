#pragma once
#include <string_view>
#include "Parser.h"

// Evaluates the AST against the input text
bool is_match(const ParsedRegex& regex, std::string_view text);