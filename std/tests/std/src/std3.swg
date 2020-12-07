#import "std"
using std.random

//#[swag.printbc]
#test
{
	var rng: Mwc
	@assert(rng.next() == 1372587705)
}