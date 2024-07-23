#global public
using Swag

// Enum
//////////////////////////////////////////////

#[Inline]
func(T) has(flags, value: T)->bool
    where Reflection.isEnumFlags(T)
{
    return flags & value != 0
}

#[Inline]
func(T) equals(flags, value: T)->bool
    where Reflection.isEnumFlags(T)
{
    return flags == value
}

#[Inline]
func(T) add(flags: &T, value: T)
    where Reflection.isEnumFlags(T)
{
    flags |= value
}

#[Inline]
func(T) remove(flags: &T, value: T)
    where Reflection.isEnumFlags(T)
{
    flags &= ~value
}

#[Inline]
func(T) set(flags: &T, value: T)
    where Reflection.isEnumFlags(T)
{
    flags = value
}

#[Inline]
func(T) toggle(flags: &T, value: T)
    where Reflection.isEnumFlags(T)
{
    flags ^= value
}
