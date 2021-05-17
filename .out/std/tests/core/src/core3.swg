using core.random

//#[swag.printbc]
#test
{
	var rng: Mwc
	@assert(rng.nextU32() == 1372587705)
}