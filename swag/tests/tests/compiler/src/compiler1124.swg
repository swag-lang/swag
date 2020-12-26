//#[swag.public]
struct RNG
{
	xx: s32
}

impl RNG
{
	func call(using self, x: s32)
	{
		//@print(cast(s64) x)
	}
}

//#[swag.public]
struct(T) toto
{
	rng: RNG
}

impl toto
{
	//#[swag.public]
	func toto1(using self, x: s32) => rng.call(x);
}

#test
{
	var t: toto'bool
	t.toto1(10)
}