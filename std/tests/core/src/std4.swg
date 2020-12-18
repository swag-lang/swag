#import "core"
using core

struct(T = random.Mwc) Random
{
	rng: T
}

impl Random
{
	func nextF32(using self)->f32 => rng.next() / cast(f32) swag.MaxU32
}

//#[swag.printbc]
#test
{
	var rng: Random
	@assert(rng.nextF32() != 0)
}