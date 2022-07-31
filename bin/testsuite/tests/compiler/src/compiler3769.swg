#test
{
    {
        x := 1.0
        y := cast(bool) x
        @assert(y)
    }

    {
        x := 1.0'f64
        y := cast(bool) x
        @assert(y)
    }

    {
        x := 0.0
        y := cast(bool) x
        @assert(!y)
    }

    {
        x := 0.0'f64
        y := cast(bool) x
        @assert(!y)
    }
}