#pragma once
enum class BuildPass
{
    Lexer,
    Syntax,
    Semantic,
    Backend,
    Full,
};
