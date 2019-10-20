#import "std"

//#[swag.printbc]
#test
{
	using std.random
	var rng: Mwc
	@assert(rng.next() == 1372587705)
}