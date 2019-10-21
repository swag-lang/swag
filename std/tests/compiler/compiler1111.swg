struct Mwc
{
	val: u32
}

impl Mwc
{
	func next(using self)->u32
	{
		val += 1
		return val
	}
}

struct(T = Mwc) Random
{
	rng: T
}

impl Random
{
	using swag
	func nextU32(using self)->u32 => rng.next()
	//func nextF32(using self)->f32 => rng.next() / cast(f32) u32_max
}

#test
{
	var rnd: Random
}