//#[swag.printbc]
#test
{
    xHi := 2'u64
	msb := cast(u32) (xHi >>,safe 1)
    @assert(msb == 1)
	retMantissa := xHi >>,safe (msb + 9)
}