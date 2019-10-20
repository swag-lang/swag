#import "std"

#test
{
	using std.random
	var rng: Mwc
	loop 10
	{
		@print(cast(s64) rng.next())
		@print("\n")
	}
}