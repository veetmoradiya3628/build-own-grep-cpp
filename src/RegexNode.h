#pragma once
#include <string_view>

class RegexNode {
public:
    virtual ~RegexNode() = default;
    virtual int match(std::string_view text) const = 0;
}

class LiteralNode: public RegexNode {
    char target;
public:
    explicit LiteralNode(char c): target(c) {}

    int match(std::string_view text) const override {
        if(!text.empty() && text[0] == target) return 1;
        return -1;
    }
}

class DigitNode: public RegexNode {
public:
    int match(std::string_view text) const override {
        if (!text.empty() && std::isdigit(text[0])) return 1;
        return -1;
    }
}