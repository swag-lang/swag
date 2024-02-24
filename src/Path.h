// ReSharper disable CppInconsistentNaming
#pragma once
#include "Utf8.h"

struct Path : Utf8
{
    Path() = default;

    Path(const char* other) :
        Utf8{other}
    {
    }

    Path(const Utf8& other) :
        Utf8{other.c_str()}
    {
    }

    Path(const filesystem::path& other) :
        Utf8{other.string()}
    {
    }

    void append(const char* str)
    {
        filesystem::path p = c_str();
        p.append(str);
        *this = p.string().c_str();
    }

    void append(const Utf8& str)
    {
        append(str.c_str());
    }

    void append(const Path& other)
    {
        append(other.c_str());
    }

    Utf8 extension() const
    {
        const filesystem::path p = c_str();
        return p.extension().string();
    }

    Path parent_path() const
    {
        const filesystem::path p = c_str();
        return p.parent_path().string().c_str();
    }

    Path filename() const
    {
        const filesystem::path p = c_str();
        return p.filename().string().c_str();
    }

    Path replace_extension(const char* ext = nullptr) const
    {
        filesystem::path p = c_str();
        if (!ext)
            return p.replace_extension().string().c_str();
        return p.replace_extension(ext).string().c_str();
    }

    operator filesystem::path() const
    {
        return filesystem::path{c_str()};
    }

    operator string() const
    {
        return string{c_str()};
    }
};

struct HashPath
{
    size_t operator()(const Path& node) const
    {
        return node.hash();
    }
};
