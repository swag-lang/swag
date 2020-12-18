#import "core"
using core

//#[swag.printbc]
#test
{
	x := environment.getProcessorCount()
	@assert(x > 0)
}