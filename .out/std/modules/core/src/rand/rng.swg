#global namespace Random
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
        rng.seedU32(Time.nowMicroseconds())
    }

    #[Swag.inline]
    func seedU32(using self, value: u32)
    {
        return rng.seedU32(value)
    }

    #[Swag.inline]
    func nextS32(using self)->s32
    {
        return bitcast(s32) rng.nextU32()
    }

    #[Swag.inline]
    func nextS64(using self)->s64
    {
        return bitcast(s64) nextU64()
    }

    #[Swag.inline]
    func nextU32(using self)->u32
    {
        return rng.nextU32()
    }

    #[Swag.inline]
    func nextU64(using self)->u64
    {
        return (cast(u64) rng.nextU32() << 32) | rng.nextU32()
    }

    // Range is [0..1[, so 1.0 will never be returned
    #[Swag.inline]
    func nextF32(using self)->f32
    {
        v := rng.nextU32()
        while v == Swag.U32.Max v = rng.nextU32()
        return v / cast(f32) Swag.U32.Max
    }

    // Range is [0..1[, so 1.0 will never be returned
    #[Swag.inline]
    func nextF64(using self)->f64
    {
        v := nextU64()
        while v == Swag.U64.Max v = nextU64()
        return v / cast(f64) Swag.U64.Max
    }

    // Returns an unsigned random value in a given range
    // 'max' is excluded
    func nextU32(using self, min, max: u32)->u32
    {
        Debug.assert(max > min, "'max' must be greater than 'min'")
        range := cast(u32) (max - min)
        value := rng.nextU32() % range
        return min + value
    }

    // Returns an unsigned random value in a given range
    // 'max' is excluded
    func nextU64(using self, min, max: u64)->u64
    {
        Debug.assert(max > min, "'max' must be greater than 'min'")
        range := cast(u64) (max - min)
        value := nextU64() % range
        return min + value
    }

    // Returns a signed random value in a given range
    // 'max' is excluded
    func nextS32(using self, min, max: s32)->s32
    {
        Debug.assert(max > min, "'max' must be greater than 'min'")
        range := cast(u32) (max - min)
        value := cast(s32) (rng.nextU32() % range)
        return min + value
    }

    // Returns a signed random value in a given range
    // 'max' is excluded
    func nextS64(using self, min, max: s64)->s64
    {
        Debug.assert(max > min, "'max' must be greater than 'min'")
        range := cast(u64) (max - min)
        value := cast(s64) (nextU64() % range)
        return min + value
    }

    // Returns a float value in a given range
    // 'max' is excluded
    func nextF32(using self, min, max: f32)->f32
    {
        Debug.assert(max > min, "'max' must be greater than 'min'")
        range := max - min
        value := nextF32()
        return min + (value * range)
    }

    // Returns a float value in a given range
    // 'max' is excluded
    func nextF64(using self, min, max: f64)->f64
    {
        Debug.assert(max > min, "'max' must be greater than 'min'")
        range := max - min
        value := nextF64()
        return min + (value * range)
    }
}
