namespace std.random
{
	#[swag.public]
	struct(T = Mwc) Random
	{
		rng: T
	}
	
	impl Random
	{
		func nextU32(using self)->u32 => rng.next()
		func nextF32(using self)->f32 => rng.next() / cast(f32) swag.u32_max
	}
}