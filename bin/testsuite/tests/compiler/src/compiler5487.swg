#global testerror "Err0152"

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

const y = Flags.Zero
func toto()
{
    y.add(y)
}
