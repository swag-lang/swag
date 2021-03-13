//#[swag.printbc]
#test
{
    {
        var x, y: u8
        x = 0
        y = 0
        @assert(x == 0)
        @assert(y == 0)
    }

    {
        var x, y: u16
        @assert(x == 0)
        @assert(y == 0)
    }

    {
        var x, y: u32
        @assert(x == 0)
        @assert(y == 0)
    }

    {
        var x, y: u64
        @assert(x == 0)
        @assert(y == 0)
    }
}