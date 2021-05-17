using core.random

//#[swag.printbc]
#test
{
	var rng: CMWC4096
    @assert(rng.nextU32() == 3266955234)
    @assert(rng.nextU32() == 2605031215)
}