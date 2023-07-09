//#[Swag.PrintBc, Swag.nooptim]
#test
{
    {
        var x: u32 = 10'u8
        var y: s8 = -1
        #[Swag.Overflow(true)]
        var z = x + y
        @assert(z == 9)
        #[Swag.Overflow(true)]
        var z1 = 10'u32 + cast(u32) -1's8
        @assert(z1 == 9)
    }

    {
        var x: u32 = 10'u8
        var y: s8 = -1
        #[Swag.Overflow(true)]
        var z = x - y
        @assert(z == 11)
        #[Swag.Overflow(true)]
        var z1 = 10'u32 - cast(u32) -1's8
        @assert(z1 == 11)
    }

    {
        var x: u32 = 10'u8
        var y: s8 = 2
        var z = x * y
        @assert(z == 20)
        var z1 = 10'u32 * 2's8
        @assert(z1 == 20)
    }

    {
        var x: u32 = 10'u8
        var y: u8 = 2
        var z = x << y
        @assert(z == 40)
        var z1 = 10'u32 << 2'u8
        @assert(z1 == 40)
    }
}