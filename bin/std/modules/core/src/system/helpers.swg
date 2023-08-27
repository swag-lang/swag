#global public
using Swag

// Enum
//////////////////////////////////////////////

#[Inline]
func(T) has(flags, value: T)->bool
    #validif Reflection.isEnumFlags(T)
{
    return flags & value != 0
}

#[Inline]
func(T) equals(flags, value: T)->bool
    #validif Reflection.isEnumFlags(T)
{
    return flags == value
}

#[Inline]
func(T) add(flags: ref T, value: T)
    #validif Reflection.isEnumFlags(T)
{
    flags |= value
}

#[Inline]
func(T) remove(flags: ref T, value: T)
    #validif Reflection.isEnumFlags(T)
{
    flags &= ~value
}

#[Inline]
func(T) set(flags: ref T, value: T)
    #validif Reflection.isEnumFlags(T)
{
    flags = value
}
