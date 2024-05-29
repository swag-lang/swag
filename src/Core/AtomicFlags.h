#pragma once

template<typename T>
struct AtomicFlags
{
    AtomicFlags() = default;

    // ReSharper disable once CppNonExplicitConvertingConstructor
    AtomicFlags(T other) :
        flags{other}
    {
    }

    AtomicFlags(const AtomicFlags& other)
    {
        flags.store(other.flags);
    }

    const AtomicFlags &operator=(T other)
    {
        flags.store(other);
        return *this;
    }

    const AtomicFlags &operator=(const AtomicFlags& other)
    {
        flags.store(other.flags);
        return *this;
    }    

    bool operator==(const AtomicFlags& other) const
    {
        return flags == other.flags;
    }

    constexpr AtomicFlags friend operator|(const AtomicFlags& a, const AtomicFlags& b)
    {
        return a.flags | b.flags;
    }

    bool has(const AtomicFlags& fl) const
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

    void add(const AtomicFlags& fl)
    {
        flags |= fl.flags;
    }

    void remove(const AtomicFlags& fl)
    {
        flags &= ~fl.flags;
    }

    std::atomic<T> flags = 0;
};
