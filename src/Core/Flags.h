#pragma once

template<typename T>
struct Flags
{
    Flags() = default;

    // ReSharper disable once CppNonExplicitConvertingConstructor
    constexpr Flags(T other) :
        flags{other}
    {
    }

    bool                   operator==(const Flags& other) const { return flags == other.flags; }
    constexpr Flags friend operator|(Flags a, Flags b) { return a.flags | b.flags; }

    SWAG_FORCE_INLINE bool has(Flags fl) const { return flags & fl.flags; }
    Flags                  with(Flags fl) const { return flags | fl.flags; }
    Flags                  mask(Flags fl) const { return flags & fl.flags; }
    Flags                  maskInvert(Flags fl) const { return flags & ~fl.flags; }
    void                   add(Flags fl) { flags |= fl.flags; }
    void                   remove(Flags fl) { flags &= ~fl.flags; }
    void                   clear() { flags = 0; }

    T flags = 0;
};
