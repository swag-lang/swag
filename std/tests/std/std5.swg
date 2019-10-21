#import "std"
using std.random

#[swag.printbc]
#test
{
	var rng: Random!Mwc
	//@assert(rng.nextF32() != 0)
}