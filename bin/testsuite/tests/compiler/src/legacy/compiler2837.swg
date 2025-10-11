//#[Swag.PrintBc]
#test
{
    let xHi = 2'u64
    let msb = cast(u32) (xHi >> 1)
    @assert(msb == 1)
    let retMantissa = xHi >> (msb + 9)
}
