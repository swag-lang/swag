//#[Swag.PrintBc]
#test
{
    {
        x := 1'u8
        @assert(cast(f64) x == 1)
        x += 1
        @assert(cast(f64) x == 2)
    }

    {
        x := 1'u16
        @assert(cast(f64) x == 1)
        x += 1
        @assert(cast(f64) x == 2)
    }

    {
        x := 1'u32
        @assert(cast(f64) x == 1)
        x += 1
        @assert(cast(f64) x == 2)
    }

    {
        x := 1'u64
        @assert(cast(f64) x == 1)
        x += 1
        @assert(cast(f64) x == 2)
    }

    /////////////////////

    {
        x := 1's8
        @assert(cast(f64) x == 1)
        x += 1
        @assert(cast(f64) x == 2)
    }

    {
        x := 1's16
        @assert(cast(f64) x == 1)
        x += 1
        @assert(cast(f64) x == 2)
    }

    {
        x := 1's32
        @assert(cast(f64) x == 1)
        x += 1
        @assert(cast(f64) x == 2)
    }

    {
        x := 1's64
        @assert(cast(f64) x == 1)
        x += 1
        @assert(cast(f64) x == 2)
    }
}