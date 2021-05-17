#global namespace random
#global public

struct(T = MWC) Rng
{
    rng: T
}

impl Rng
{
    // Seed the rng with the current time
    func seedNow(using self)
    {
        rng.seedU32(time.nowMilliseconds(), time.nowMicroseconds())
    }

    #[swag.inline]
    func seedU32(using self, value: u32)
    {
        return rng.seedU32(value)
    }

    #[swag.inline]
    func nextS32(using self)->s32
    {
        return bitcast(s32) rng.nextU32()
    }

    #[swag.inline]
    func nextS64(using self)->s64
    {
        return bitcast(s64) nextU64()
    }

    #[swag.inline]
    func nextU32(using self)->u32
    {
        return rng.nextU32()
    }

    #[swag.inline]
    func nextU64(using self)->u64
    {
        return (cast(u64) rng.nextU32() << 32) | rng.nextU32()
    }

    #[swag.inline]
    func nextF32(using self)->f32
    {
        return rng.nextU32() / cast(f32) swag.U32.Max
    }

    #[swag.inline]
    func nextF64(using self)->f64
    {
        return nextU64() / cast(f64) swag.U64.Max
    }

    func nextS32(using self, min, max: s32)->s32
    {
        debug.assert(max > min, "'max' must be greater than 'min'")
        range := cast(u32) (max - min)
        value := cast(s32) (rng.nextU32() % range)
        return min + value
    }

    func nextS64(using self, min, max: s64)->s64
    {
        debug.assert(max > min, "'max' must be greater than 'min'")
        range := cast(u64) (max - min)
        value := cast(s64) (nextU64() % range)
        return min + value
    }
}
