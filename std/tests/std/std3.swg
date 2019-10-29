#import "std"
using std.crypto

//#[swag.printbc]
#test
{
	var rng: Mwc
	@assert(rng.next() == 1372587705)
}