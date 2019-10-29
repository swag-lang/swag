#import "std"
using std

//#[swag.printbc]
#test
{
	x := environment.getProcessorCount()
	@assert(x > 0)
}