//#[Swag.PrintBc]
#test
{
    {
        x := 0's8
        x -= 1
        var y: f64 = x
        @assert(y == -1)
    }

    {
        x := 0's16
        x -= 1
        var y: f64 = x
        @assert(y == -1)
    }

    {
        x := 0'u8
        x += 1
        var y: f64 = x
        @assert(y == 1)
    }

    {
        x := 0'u16
        x += 1
        var y: f64 = x
        @assert(y == 1)
    }
}