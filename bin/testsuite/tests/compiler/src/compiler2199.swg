//#[Swag.PrintBc]
#test
{
    {
        var x: u32 = 10'u8
        let y: s8 = -1
        #[Swag.CanOverflow(true)]
        x += cast(u32) y
        @assert(x == 9)
    }

    {
        var x: u32 = 10'u8
        let y: u8 = 1
        x |= y
        @assert(x == 11)
    }

    {
        var x: u32 = 10'u8
        let y: u8 = 2
        x /= y
        @assert(x == 5)
    }

    {
        var x: u32 = 10'u8
        let y: u8 = 2
        x <<= y
        @assert(x == 40)
    }
}