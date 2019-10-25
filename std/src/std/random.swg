namespace std.random
{
	#[swag.public]
	struct(T = Mwc) Random
	{
		rng: T
	}
	
	impl Random
	{
		#[swag.public]
		{
			func nextS32(using self)->s32 => cast(s32) rng.next()
			func nextU32(using self)->u32 => rng.next()		
			func nextU64(using self)->u64 => (cast(u64) rng.next() << 32) | rng.next()
			func nextF32(using self)->f32 => rng.next() / cast(f32) swag.u32_max
			func nextF64(using self)->f64 => nextU64(self) / cast(f64) swag.u64_max
		}
	}
}