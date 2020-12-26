#import "core"
using core.random

//#[swag.printbc]
#test
{
	var rng: Mwc
	@assert(rng.next() == 1372587705)
}