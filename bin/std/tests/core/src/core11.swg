
using Core

//#[Swag.PrintBc]
#test
{
	let x = Hardware.getProcessorCount()
	@assert(x > 0)
}