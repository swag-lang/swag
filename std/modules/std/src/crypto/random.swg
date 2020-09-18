namespace std.crypto
{
	public struct(T = Mwc) Random
	{
		rng: T
	}

	public impl Random
	{
        func seed(using self, value: u32) => rng.seed(value)

        func nextS32(using self)->s32 => cast(s32) rng.next()
        func nextS64(using self)->s64 => cast(s64) nextU64(self)
        func nextU32(using self)->u32 => rng.next()
        func nextU64(using self)->u64 => (cast(u64) rng.next() << 32) | rng.next()
        func nextF32(using self)->f32 => rng.next() / cast(f32) swag.MaxU32
        func nextF64(using self)->f64 => nextU64(self) / cast(f64) swag.MaxU64

        func nextS32(using self, min, max: s32)->s32
        {
            debug.assert(max > min)
            range := cast(u32) (max - min)
            value := cast(s32) (rng.next() % range)
            return min + value
        }

        func nextS64(using self, min, max: s64)->s64
        {
            debug.assert(max > min)
            range := cast(u64) (max - min)
            value := cast(s64) (nextU64() % range)
            return min + value
        }
	}
}