#import "std"
using std.random

//#[swag.printbc]
#test
{
	var rng: Random!Mwc
	@assert(rng.nextU32() != 0)
}