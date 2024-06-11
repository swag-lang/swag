func roundDownToPowerOf2A(x: u32)->u32
{
    var i = 1'u32
    while (i << 1) <= x do
        i <<= 1
    return i
}

func roundDownToPowerOf2B(x: u32)->u32
{
    let i = 31 - @bitcountlz(x)
    return 1'u32 << i
}

#test
{
    @assert(roundDownToPowerOf2A(5) == roundDownToPowerOf2B(5))
    @assert(roundDownToPowerOf2A(37) == roundDownToPowerOf2B(37))
    @assert(roundDownToPowerOf2A(1555) == roundDownToPowerOf2B(1555))
}