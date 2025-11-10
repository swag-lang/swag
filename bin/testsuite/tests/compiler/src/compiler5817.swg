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
    @assert(totoMin(127'u8, 255'u8) == 127)
    @assert(totoMin(127'u16, 255'u16) == 127)
    @assert(totoMin(127'u32, 255'u32) == 127)
    @assert(totoMin(127'u64, 255'u64) == 127)
    @assert(totoMin(127'u8, 55'u8) == 55)
    @assert(totoMin(127'u16, 55'u16) == 55)
    @assert(totoMin(127'u32, 55'u32) == 55)
    @assert(totoMin(127'u64, 55'u64) == 55)

    @assert(totoMax(127'u8, 255'u8) == 255)
    @assert(totoMax(127'u16, 255'u16) == 255)
    @assert(totoMax(127'u32, 255'u32) == 255)
    @assert(totoMax(127'u64, 255'u64) == 255)
    @assert(totoMax(127'u8, 55'u8) == 127)
    @assert(totoMax(127'u16, 55'u16) == 127)
    @assert(totoMax(127'u32, 55'u32) == 127)
    @assert(totoMax(127'u64, 55'u64) == 127)
}
