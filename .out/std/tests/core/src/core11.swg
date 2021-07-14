
using Core

//#[Swag.PrintBc]
#test
{
	x := Env.getProcessorCount()
	@assert(x > 0)
}