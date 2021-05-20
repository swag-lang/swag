
using core

//#[Swag.printbc]
#test
{
	x := env.getProcessorCount()
	@assert(x > 0)
}