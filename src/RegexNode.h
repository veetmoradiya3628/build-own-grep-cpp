#pragma once
#include <string_view>

class RegexNode
{
public:
    virtual ~RegexNode() = default;
    virtual int match(std::string_view text) const = 0;
};

class LiteralNode : public RegexNode
{
    char target;

public:
    explicit LiteralNode(char c) : target(c) {}

    int match(std::string_view text) const override
    {
        if (!text.empty() && text[0] == target)
            return 1;
        return -1;
    }
};

class DigitNode : public RegexNode
{
public:
    int match(std::string_view text) const override
    {
        if (!text.empty() && std::isdigit(text[0]))
            return 1;
        return -1;
    }
};

class WordNode : public RegexNode
{
public:
    int match(std::string_view text) const override
    {
        if (text.empty())
            return -1;

        char c = text[0];
        bool is_word = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || (c == '_');
        if (is_word)
            return 1;
        return -1;
    }
};

class PositiveGroupNode : public RegexNode
{
    std::string allowed_chars;

public:
    explicit PositiveGroupNode(std::string chars) : allowed_chars(std::move(chars)) {}

    int match(std::string_view text) const override
    {
        if (text.empty())
            return -1;
        if (allowed_chars.find(text[0]) != std::string::npos)
        {
            return 1;
        }
        return -1;
    }
};