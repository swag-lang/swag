#pragma once
struct Utf8;

struct Path : filesystem::path
{
    Path() = default;

    Path(const char* other)
        : path{other}
    {
    }

    Path(const Utf8& other)
        : path{other.c_str()}
    {
    }

    Path(const path& other)
        : path{other}
    {
    }
};

struct HashPath
{
    size_t operator()(const Path& node) const
    {
        return Utf8(node.string()).hash();
    }
};
