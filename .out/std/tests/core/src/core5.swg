
using Core.Random

//#[Swag.PrintBc]
#test
{
	var rng: Rng
	loop 100
	{
		v := rng.nextS32(10, 50)
		//@print(cast(s64) v)
		//@print("\n")
		@assert(v >= 10 and v < 50)
	}
}