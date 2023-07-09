
using Core

//#[Swag.PrintBc]
#test
{
	var x = Hardware.getProcessorCount()
	@assert(x > 0)
}