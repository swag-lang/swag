//#[Swag.PrintBc]
func toto()
{
    {
        var x = 0'u8
        var y = x orelse 1
        @assert(y == 1)
    }

    {
        var x = 0'u16
        var y = x orelse 1
        @assert(y == 1)
    }

    {
        var x = 0'u32
        var y = x orelse 1
        @assert(y == 1)
    }

    {
        var x = 0'rune
        var y = x orelse 1'rune
        @assert(y == 1)
    }
}

#test
{
    toto()
}