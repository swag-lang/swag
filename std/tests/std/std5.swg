#import "std"
using std.random

//#[swag.printbc]
#test
{
	var rng: Random!Mwc
	@print(rng.nextF64())
	@print("\n")
	@print(rng.nextF64())
}