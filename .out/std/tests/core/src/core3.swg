using core.random

//#[swag.printbc]
#test
{
	var rng: MWC
	@assert(rng.nextU32() != 0)
}