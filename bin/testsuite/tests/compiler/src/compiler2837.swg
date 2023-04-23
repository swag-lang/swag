//#[Swag.PrintBc]
#test
{
    xHi := 2'u64
	msb := cast(u32) (xHi >>,over 1)
    @assert(msb == 1)
	retMantissa := xHi >>,over (msb + 9)
}