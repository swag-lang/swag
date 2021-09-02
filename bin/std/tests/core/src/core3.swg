using Core.Random

//#[Swag.PrintBc]
#test
{
	var rng: MWC
	@assert(rng.nextU32() != 0)
}