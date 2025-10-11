//#global skip
#[Swag.EnumFlags]
enum FlagsA
{
    Zero = 0
    A    = 1
}

#[Swag.EnumFlags]
enum FlagsB
{
    Zero = 0
    A    = 2
}

#[Swag.Inline]
func(T) has(flags, value: T)->bool
{
    return flags & value ? true : false
}

var af:  FlagsA = .A
var af1: FlagsB = .A

func toto()
{
    if af.has(.A)
    {
        @print("a")
    }

    if af1.has(.A)
    {
        @print("b")
    }
}
