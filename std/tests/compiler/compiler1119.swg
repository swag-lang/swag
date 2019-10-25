namespace A.B
{
	#[swag.public]
	struct Mwc
	{
		x: 		u32 = 1234567
		carry: 	u32 = 1234567
	}
	
	impl Mwc
	{
		func next(using self)->u32 => 1
	}
}

namespace A.B
{
	#[swag.public] // toto
	struct(T = Mwc) Random
	{
		rng: T
	}
	
	impl Random
	{
		#[swag.public]
		{
			func nextS32(using self)->s32 => cast(s32) rng.next()
		}
	}
}

using A.B
#test
{
	var rng: Random
	rng.nextS32()
}
