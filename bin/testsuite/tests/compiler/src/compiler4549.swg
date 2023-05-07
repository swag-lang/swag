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

func toto(x: Flags)
{
    var y: Flags
    y.add(x)
}
