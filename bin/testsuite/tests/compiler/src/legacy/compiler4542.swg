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
    flags &= value
    flags ^= value
}

#test
{
    var x: Flags
    //x.add(.A)
}
