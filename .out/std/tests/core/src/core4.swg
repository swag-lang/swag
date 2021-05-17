
using core

struct(T = random.MWC) Random
{
	rng: T
}

impl Random
{
	func nextF32(using self)->f32 => rng.nextU32() / cast(f32) swag.U32.Max
}

//#[swag.printbc]
#test
{
	var rng: Random
	@assert(rng.nextF32() != 0)
}