#include "Parser.h"

ParsedRegex parse_pattern(std::string_view pattern) {
    ParsedRegex regex;
    
    for (size_t i = 0; i < pattern.length(); ++i) {
        char c = pattern[i];
        if (c == '\\') {
            if (i + 1 < pattern.length()) {
                char next_c = pattern[++i];
                if (next_c == 'd') {
                    regex.nodes.push_back(std::make_unique<DigitNode>());
                } else if (next_c == 'w'){
                    regex.nodes.push_back(std::make_unique<WordNode>());
                } else {
                    regex.nodes.push_back(std::make_unique<LiteralNode>(next_c));
                }
            } else {
                regex.nodes.push_back(std::make_unique<LiteralNode>('\\'));
            }
        } else {
            regex.nodes.push_back(std::make_unique<LiteralNode>(c));
        }
    }
    
    return regex;
}