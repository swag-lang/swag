
using Core

//#[Swag.PrintBc]
#test
{
	x := Hardware.getProcessorCount()
	@assert(x > 0)
}