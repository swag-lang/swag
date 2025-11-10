//#[Swag.PrintBc]
#test
{
    {
        var x = 0's8
        x -= 1
        let y: f64 = x
        @assert(y == -1)
    }

    {
        var x = 0's16
        x -= 1
        let y: f64 = x
        @assert(y == -1)
    }

    {
        var x = 0'u8
        x += 1
        let y: f64 = x
        @assert(y == 1)
    }

    {
        var x = 0'u16
        x += 1
        let y: f64 = x
        @assert(y == 1)
    }
}
