#global public
using Swag

// Enum
//////////////////////////////////////////////
/*
func(T) has(flags, value: T)->bool
    #validif Reflection.isEnumFlags(T)
{
    return flags & value != 0
}

func(T) add(flags: ref T, value: T)
    #validif Reflection.isEnumFlags(T)
{
    flags |= value
}*/