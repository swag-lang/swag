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

    [[nodiscard]] SWAG_FORCE_INLINE bool has(Flags fl) const { return flags & fl.flags; }
    [[nodiscard]] Flags                  with(Flags fl) const { return flags | fl.flags; }
    [[nodiscard]] Flags                  mask(Flags fl) const { return flags & fl.flags; }
    [[nodiscard]] Flags                  maskInvert(Flags fl) const { return flags & ~fl.flags; }
    void                                 add(Flags fl) { flags |= fl.flags; }
    void                                 remove(Flags fl) { flags &= ~fl.flags; }

    T flags = 0;
};
