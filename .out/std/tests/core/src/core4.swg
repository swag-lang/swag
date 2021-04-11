
using core

struct(T = random.Mwc) Random
{
	rng: T
}

impl Random
{
	func nextF32(using self)->f32 => rng.next() / cast(f32) swag.U32.Max
}

//#[swag.printbc]
#test
{
	var rng: Random
	@assert(rng.nextF32() != 0)
}