using Core

struct(T = Random.MWC) Rdom
{
	rng: T
}

impl Rdom
{
	func nextF32(using self)->f32 => rng.nextU32() / cast(f32) Swag.U32.Max
}

//#[Swag.printbc]
#test
{
	var rng: Rdom
	@assert(rng.nextF32() != 0)
}