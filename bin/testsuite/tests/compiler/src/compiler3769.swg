#test
{
    {
        let x = 1.0
        let y = cast(bool) x
        @assert(y)
    }

    {
        let x = 1.0'f64
        let y = cast(bool) x
        @assert(y)
    }

    {
        let x = 0.0
        let y = cast(bool) x
        @assert(!y)
    }

    {
        let x = 0.0'f64
        let y = cast(bool) x
        @assert(!y)
    }
}
