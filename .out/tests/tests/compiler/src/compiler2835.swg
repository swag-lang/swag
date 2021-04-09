//#[swag.printbc]
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

//#[swag.printbc]
func toto1(x: u8)
{
    if (x&0xFF) == "e"'u8
    {
        @assert(false)
    }
}