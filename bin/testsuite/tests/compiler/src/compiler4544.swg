#[Swag.EnumFlags]
enum Flags
{
    Zero = 0
    A
    B
    C
}

func add(flags: &Flags, value: Flags)
{
    flags |= value
}

#test
{
    var x: Flags
    @assert(x == .Zero)
    x.add(.A)
    @assert(x == .A)
}
