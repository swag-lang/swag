#global skip
#[Swag.EnumFlags]
enum Flags
{
    Zero = 0
    A
    B
    C
}

func(T) add(flags: ref T, value: T)
{
    flags |= value
}

#test
{
    var x: Flags
    x.add(.A)
}