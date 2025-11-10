//#[Swag.PrintBc, Swag.nooptim]
#test
{
    {
        let x: u32 = 10'u8
        let y: s8 = -1
        #[Swag.CanOverflow(true)]
        let z = x + y
        @assert(z == 9)
        #[Swag.CanOverflow(true)]
        let z1 = 10'u32 + cast(u32) -1's8
        @assert(z1 == 9)
    }

    {
        let x: u32 = 10'u8
        let y: s8 = -1
        #[Swag.CanOverflow(true)]
        let z = x - y
        @assert(z == 11)
        #[Swag.CanOverflow(true)]
        let z1 = 10'u32 - cast(u32) -1's8
        @assert(z1 == 11)
    }

    {
        let x: u32 = 10'u8
        let y: s8 = 2
        let z  = x * y
        @assert(z == 20)
        let z1 = 10'u32 * 2's8
        @assert(z1 == 20)
    }

    {
        let x: u32 = 10'u8
        let y: u8 = 2
        let z  = x << y
        @assert(z == 40)
        let z1 = 10'u32 << 2'u8
        @assert(z1 == 40)
    }
}
