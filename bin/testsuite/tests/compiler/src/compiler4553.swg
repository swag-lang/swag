#global skip
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
func(T) has(flags, value: T)->bool
{
    return flags & value ? true : false
}

struct A
{
    af: FlagsA = .A
}

struct B
{
    using a: A
    af1: FlagsB = .A
}

impl B
{
    mtd toto()
    {
        if af.has(.A)
        {
            @print("a")
        }

        if af1.has(.A)
        {
            @print("b")
        }

        /*if af1.has(.A)
        {
            @print("b")
        }*/
    }
}

#test
{
    var b: B
    b.toto()
}