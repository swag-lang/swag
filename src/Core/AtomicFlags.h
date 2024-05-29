#pragma once

template<typename T>
struct AtomicFlags
{
    AtomicFlags() = default;

    // ReSharper disable once CppNonExplicitConvertingConstructor
    constexpr AtomicFlags(T other) :
        flags{other}
    {
    }

    bool operator==(const AtomicFlags& other) const
    {
        return flags == other.flags;
    }

    constexpr AtomicFlags friend operator|(AtomicFlags a, AtomicFlags b)
    {
        return a.flags | b.flags;
    }

    bool has(AtomicFlags fl) const
    {
        return flags & fl.flags;
    }
    AtomicFlags with(AtomicFlags fl) const
    {
        return flags | fl.flags;
    }

    AtomicFlags mask(AtomicFlags fl) const
    {
        return flags & fl.flags;
    }

    AtomicFlags maskInvert(AtomicFlags fl) const
    {
        return flags & ~fl.flags;
    }

    void add(AtomicFlags fl)
    {
        flags |= fl.flags;
    }

    void remove(AtomicFlags fl)
    {
        flags &= ~fl.flags;
    }

    T flags = 0;
};
