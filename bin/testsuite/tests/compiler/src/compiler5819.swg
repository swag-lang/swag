func(T) totoMin(a, b: T)->T
{
    return @min(a, b)
}

func(T) totoMax(a, b: T)->T
{
    return @max(a, b)
}

#[Swag.Optimize(.All, false)]
//#[Swag.PrintBc]
#test
{
    @assert(totoMin(27's8, 125's8) == 27)
    @assert(totoMin(27's16, 125's16) == 27)
    @assert(totoMin(27's32, 125's32) == 27)
    @assert(totoMin(27's64, 125's64) == 27)

    @assert(totoMin(127's8, 55's8) == 55)
    @assert(totoMin(127's16, 55's16) == 55)
    @assert(totoMin(127's32, 55's32) == 55)
    @assert(totoMin(127's64, 55's64) == 55)

    @assert(totoMax(27's8, 55's8) == 55)
    @assert(totoMax(27's16, 55's16) == 55)
    @assert(totoMax(27's32, 55's32) == 55)
    @assert(totoMax(27's64, 55's64) == 55)

    @assert(totoMax(127's8, 55's8) == 127)
    @assert(totoMax(127's16, 55's16) == 127)
    @assert(totoMax(127's32, 55's32) == 127)
    @assert(totoMax(127's64, 55's64) == 127)
}
