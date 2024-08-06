#global testerror "Err0609"

#[Swag.EnumFlags]
enum FlagsA
{
    Zero = 0
    A = 1
}

#[Swag.EnumFlags]
enum FlagsB
{
    Zero = 0
    A = 2
}

#[Swag.Inline]
func(T) has(flags: T, value: T)->bool
{
    return flags & value ? true : false
}

func toto()
{
    var af: FlagsA = .A
    if af.has(FlagsB.A)
    {
    }
}
