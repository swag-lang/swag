
using core

//#[swag.printbc]
#test
{
	x := env.getProcessorCount()
	@assert(x > 0)
}