#[Swag.EnumFlags]
enum Flags
{
    Zero = 0
    A
    B
    C
}

#[Swag.Strict]
alias A = Flags

func add(flags: &A, value: A)
{
    flags |= value
    flags &= value
    flags ^= value
}

#test
{
    var x: A
    //x.add(.A)
}
