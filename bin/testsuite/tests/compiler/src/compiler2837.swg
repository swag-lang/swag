//#[Swag.PrintBc]
#test
{
    var xHi = 2'u64
	var msb = cast(u32) (xHi >> 1)
    @assert(msb == 1)
	var retMantissa = xHi >> (msb + 9)
}