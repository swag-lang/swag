#import "std"

//#[swag.printbc]
#test
{
	using std.random
	var rng: Mwc
	
	//rng.seed(12345678)
	//loop 10
	{
		@print(cast(s64) rng.next())
		@print("\n")
	}
}