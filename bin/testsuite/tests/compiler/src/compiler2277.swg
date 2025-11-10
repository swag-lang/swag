//#[Swag.PrintBc]
func toto()
{
    {
        let x = 0'u8
        let y = x orelse 1
        @assert(y == 1)
    }

    {
        let x = 0'u16
        let y = x orelse 1
        @assert(y == 1)
    }

    {
        let x = 0'u32
        let y = x orelse 1
        @assert(y == 1)
    }

    {
        let x = 0'rune
        let y = x orelse 1'rune
        @assert(y == 1)
    }
}

#test
{
    toto()
}
