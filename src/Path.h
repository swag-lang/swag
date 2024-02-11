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

    void append(const char* str)
    {
        path::append(str);
    }

    void append(const Utf8& str)
    {
        path::append(str.c_str());
    }

    void append(const Path& path)
    {
        path::append(path.string());
    }
};

struct HashPath
{
    size_t operator()(const Path& node) const
    {
        return Utf8(node.string()).hash();
    }
};
