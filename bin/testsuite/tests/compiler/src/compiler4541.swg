#[Swag.EnumFlags]
enum Flags
{
    Zero = 0
    A
    B
    C
}

func(T) has(flags, value: T)->bool
    where #typeof(T).kind == Swag.TypeInfoKind.Enum
{
    return flags & value != 0
}

#test
{
    var x: Flags
    x |= .A
    @assert(x.has(.A))
    @assert(!x.has(.B))
}
