// ReSharper disable CppInconsistentNaming
// ReSharper disable CppNonExplicitConvertingConstructor
#pragma once
#include "Core/Utf8.h"

struct Path : Utf8
{
    Path() = default;

    Path(const char* other) :
        Utf8{other}
    {
    }

    Path(const Utf8& other) :
        Utf8{other.cstr()}
    {
    }

    Path(const std::filesystem::path& other) :
        Utf8{other.string()}
    {
    }

    void append(const char* str)
    {
        std::filesystem::path p = cstr();
        p.append(str);
        *this = p.string().c_str();
    }

    void append(const Utf8& str)
    {
        append(str.cstr());
    }

    void append(const Path& other)
    {
        append(other.cstr());
    }

    Utf8 extension() const
    {
        const std::filesystem::path p = cstr();
        return p.extension().string();
    }

    Path parent_path() const
    {
        const std::filesystem::path p = cstr();
        return p.parent_path().string().c_str();
    }

    Path filename() const
    {
        const std::filesystem::path p = cstr();
        return p.filename().string().c_str();
    }

    Path replace_extension(const char* ext = nullptr) const
    {
        std::filesystem::path p = cstr();
        if (!ext)
            return p.replace_extension().string().c_str();
        return p.replace_extension(ext).string().c_str();
    }

    // ReSharper disable once CppNonExplicitConversionOperator
    operator std::filesystem::path() const
    {
        return std::filesystem::path{cstr()};
    }

    // ReSharper disable once CppNonExplicitConversionOperator
    operator std::string() const
    {
        return std::string{cstr()};
    }
};

struct HashPath
{
    size_t operator()(const Path& node) const
    {
        return node.hash();
    }
};
