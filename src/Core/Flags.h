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

template<typename T>
struct EnumFlags
{
    using U = std::underlying_type_t<T>;

    EnumFlags() = default;

    // implicit on purpose, like your original
    EnumFlags(T other) :
        flags{static_cast<U>(other)}
    {
    }

    bool             operator==(const EnumFlags& other) const { return flags == other.flags; }
    friend EnumFlags operator|(EnumFlags a, EnumFlags b) { return EnumFlags{static_cast<T>(a.flags | b.flags)}; }

    SWAG_FORCE_INLINE bool has(EnumFlags fl) const { return (flags & fl.flags) != 0; }
    EnumFlags              with(EnumFlags fl) const { return EnumFlags{static_cast<T>(flags | fl.flags)}; }
    EnumFlags              mask(EnumFlags fl) const { return EnumFlags{static_cast<T>(flags & fl.flags)}; }
    EnumFlags              maskInvert(EnumFlags fl) const { return EnumFlags{static_cast<T>(flags & ~fl.flags)}; }
    void                   add(EnumFlags fl) { flags |= fl.flags; }
    void                   remove(EnumFlags fl) { flags &= ~fl.flags; }
    void                   clear() { flags = 0; }

    U flags = 0;
};
