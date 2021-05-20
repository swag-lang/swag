
using Core

//#[Swag.printbc]
#test
{
	x := Env.getProcessorCount()
	@assert(x > 0)
}