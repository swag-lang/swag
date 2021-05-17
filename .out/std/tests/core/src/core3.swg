using core.random

//#[swag.printbc]
#test
{
	var rng: Mwc
	@assert(rng.nextU32() != 0)
}