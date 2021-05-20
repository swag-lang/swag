using core.random

//#[Swag.printbc]
#test
{
	var rng: MWC
	@assert(rng.nextU32() != 0)
}