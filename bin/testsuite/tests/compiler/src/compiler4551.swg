#[Swag.EnumFlags]
enum Flags
{
    Zero = 0
    A
    B
    C
}

func add(flags: ref Flags, value: Flags)
{
    flags |= value
}

func toto(add: Flags)
{
    var y: Flags
    y.add(add)
    @assert(y == .A)
}

#test
{
    toto(.A)
}