//#[swag.printbc]
#test
{
    xHi := 2'u64
	msb := cast(u32) (xHi >>% 1)
    @assert(msb == 1)
	retMantissa := xHi >>% (msb + 9)
}