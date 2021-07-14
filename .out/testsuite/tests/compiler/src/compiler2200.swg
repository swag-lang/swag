//#[Swag.PrintBc, Swag.nooptim]
#test
{
    {
        var x: u32 = 10'u8
        var y: s8 = -1
        #[Swag.Safety("overflow", false)]
        z := x + y
        @assert(z == 9)
        #[Swag.Safety("overflow", false)]
        z1 := 10'u32 + -1's8
        @assert(z1 == 9)
    }

    {
        var x: u32 = 10'u8
        var y: s8 = -1
        #[Swag.Safety("overflow", false)]
        z := x - y
        @assert(z == 11)
        #[Swag.Safety("overflow", false)]
        z1 := 10'u32 - -1's8
        @assert(z1 == 11)
    }

    {
        var x: u32 = 10'u8
        var y: s8 = 2
        z := x * y
        @assert(z == 20)
        z1 := 10'u32 * 2's8
        @assert(z1 == 20)
    }

    {
        var x: u32 = 10'u8
        var y: u8 = 2
        z := x << y
        @assert(z == 40)
        z1 := 10'u32 << 2'u8
        @assert(z1 == 40)
    }
}