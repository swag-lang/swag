#import "std"
using std.random

//#[swag.printbc]
#test
{
	var rng: Random!Mwc
	loop 10
	{
		@print(cast(s64) rng.nextS32(10, 50))
		@print("\n")
	}
}