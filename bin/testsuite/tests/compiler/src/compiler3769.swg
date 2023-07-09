#test
{
    {
        var x = 1.0
        var y = cast(bool) x
        @assert(y)
    }

    {
        var x = 1.0'f64
        var y = cast(bool) x
        @assert(y)
    }

    {
        var x = 0.0
        var y = cast(bool) x
        @assert(!y)
    }

    {
        var x = 0.0'f64
        var y = cast(bool) x
        @assert(!y)
    }
}