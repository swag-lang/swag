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

    constexpr AtomicFlags(const AtomicFlags& other)
    {
        flags.store(other.flags);
    }

    AtomicFlags& operator=(T other)
    {
        flags.store(other);
        return *this;
    }

    AtomicFlags& operator=(const AtomicFlags& other)
    {
        flags.store(other.flags);
        return *this;
    }    

    bool operator==(const AtomicFlags& other) const
    {
        return flags == other.flags;
    }

    constexpr AtomicFlags friend operator|(AtomicFlags a, AtomicFlags b)
    {
        return a.flags | b.flags;
    }

    constexpr bool has(const AtomicFlags& fl) const
    {
        return flags & fl.flags;
    }

    AtomicFlags with(const AtomicFlags& fl) const
    {
        return flags | fl.flags;
    }

    AtomicFlags mask(const AtomicFlags& fl) const
    {
        return flags & fl.flags;
    }

    AtomicFlags maskInvert(const AtomicFlags& fl) const
    {
        return flags & ~fl.flags;
    }

    constexpr void add(const AtomicFlags& fl)
    {
        flags |= fl.flags;
    }

    void remove(const AtomicFlags& fl)
    {
        flags &= ~fl.flags;
    }

    void clear()
    {
        flags = 0;
    }

    std::atomic<T> flags = 0;
};
