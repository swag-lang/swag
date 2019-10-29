#import "std"

struct(T = std.crypto.Mwc) Random
{
	rng: T
}
	
impl Random
{
	func nextF32(using self)->f32 => rng.next() / cast(f32) swag.u32_max
}

//#[swag.printbc]
#test
{
	var rng: Random
	@assert(rng.nextF32() != 0)
}