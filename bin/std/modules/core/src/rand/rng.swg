#global namespace Random
#global public

alias DefaultRng = Rng'MWC

struct(T = MWC) Rng
{
    rng: T
}

impl Rng
{
    // Seed the rng with the current time
    mtd seedNow()
    {
        rng.seedU32(Time.nowMicroseconds())
    }

    #[Swag.Inline]
    mtd seedU32(value: u32)
    {
        return rng.seedU32(value)
    }

    #[Swag.Inline]
    mtd nextS32()->s32
    {
        return bitcast(s32) rng.nextU32()
    }

    #[Swag.Inline]
    mtd nextS64()->s64
    {
        return bitcast(s64) nextU64()
    }

    #[Swag.Inline]
    mtd nextU32()->u32
    {
        return rng.nextU32()
    }

    #[Swag.Inline]
    mtd nextU64()->u64
    {
        return (cast(u64) rng.nextU32() << 32) | rng.nextU32()
    }

    // Range is [0..1[, so 1.0 will never be returned
    #[Swag.Inline]
    mtd nextF32()->f32
    {
        v := rng.nextU32()
        while v == Swag.U32.Max v = rng.nextU32()
        return v / cast(f32) Swag.U32.Max
    }

    // Range is [0..1[, so 1.0 will never be returned
    #[Swag.Inline]
    mtd nextF64()->f64
    {
        v := nextU64()
        while v == Swag.U64.Max v = nextU64()
        return v / cast(f64) Swag.U64.Max
    }

    // Returns an unsigned random value in a given range
    // 'max' is excluded
    mtd nextU32(min, max: u32)->u32
    {
        Debug.assert(max > min, "'max' must be greater than 'min'")
        range := cast(u32) (max - min)
        value := rng.nextU32() % range
        return min + value
    }

    // Returns an unsigned random value in a given range
    // 'max' is excluded
    mtd nextU64(min, max: u64)->u64
    {
        Debug.assert(max > min, "'max' must be greater than 'min'")
        range := cast(u64) (max - min)
        value := nextU64() % range
        return min + value
    }

    // Returns a signed random value in a given range
    // 'max' is excluded
    mtd nextS32(min, max: s32)->s32
    {
        Debug.assert(max > min, "'max' must be greater than 'min'")
        range := cast(u32) (max - min)
        value := cast(s32) (rng.nextU32() % range)
        return min + value
    }

    // Returns a signed random value in a given range
    // 'max' is excluded
    mtd nextS64(min, max: s64)->s64
    {
        Debug.assert(max > min, "'max' must be greater than 'min'")
        range := cast(u64) (max - min)
        value := cast(s64) (nextU64() % range)
        return min + value
    }

    // Returns a float value in a given range
    // 'max' is excluded
    mtd nextF32(min, max: f32)->f32
    {
        Debug.assert(max > min, "'max' must be greater than 'min'")
        range := max - min
        value := nextF32()
        return min + (value * range)
    }

    // Returns a float value in a given range
    // 'max' is excluded
    mtd nextF64(min, max: f64)->f64
    {
        Debug.assert(max > min, "'max' must be greater than 'min'")
        range := max - min
        value := nextF64()
        return min + (value * range)
    }

    // Returns a random bool
    mtd nextBool()->bool
    {
        return nextU32(0, 2) == 0 ? false : true
    }

    // Change the order of a slice
    mtd(V) shuffle(slice: [..] V)
    {
        countOf := @countof(slice)
        dataOf  := @dataof(slice)
        loop i: slice
        {
            o := nextU64(0, countOf)
            Core.swap(dataOf + i, dataOf + o)
        }
    }
}
