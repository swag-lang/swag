//#[Swag.printbc]
#test
{
    x := 0'u8
    if (x|32) == "e"'u8
    {
        @assert(false)
    }

    toto(1)
    toto1(1)
}

func toto(x: u8)
{
    if (x|32) == "e"'u8
    {
        @assert(false)
    }
}

//#[Swag.printbc]
func toto1(x: u8)
{
    @assert(x & 0xFF == x)
    @assert(0xFFFFFFFF & x == x)
    @assert(x | 0'u32 == x)
    @assert(0'u32 | x == x)
    @assert(x | 0xFF == 0xFF)
    @assert(0xFF | x == 0xFF)
}