#global namespace Random
#global public

struct(T = MWC) Rng
{
    rng:          T
    seedDone:     bool
}

impl Rng
{
    // Seed random generator with a 32 bits value
    #[Swag.Inline]
    mtd seedU32(seed: u32)
    {
        .seedDone = true
        #if #defined(T.seedU32) do
            return .rng.seedU32(seed)
        #else do
            return .rng.seedU64(cast(u64) seed)
    }

    // Seed random generator with a 64 bits value
    #[Swag.Inline]
    mtd seedU64(seed: u64)
    {
        .seedDone = true
        #if #defined(T.seedU64) do
            return .rng.seedU64(seed)
        #else do
            return .rng.seedU32(cast #wrap (u32) seed)
    }

    // Returns an unsigned 32 bits random value
    #[Swag.Inline, Swag.Overload]
    mtd nextU32()->u32
    {
        if !.seedDone do
            .seedU32(Time.nowMilliseconds())

        #if #defined(T.nextU32) do
            return .rng.nextU32()
        #else do
            return cast #wrap (u32) .rng.nextU64()
    }

    // Returns an unsigned 64 bits random value
    #[Swag.Inline, Swag.Overload]
    mtd nextU64()->u64
    {
        if !.seedDone do
            .seedU32(Time.nowMilliseconds())

        #if #defined(T.nextU64) do
            return .rng.nextU64()
        #else do
            return (cast(u64) .rng.nextU32() << 32) | .rng.nextU32()
    }

    // Seed the rng with the current time
    mtd seedNow()
    {
        .seedU32(Time.nowMicroseconds())
    }

    // Returns a signed 32 bits random value
    #[Swag.Inline, Swag.Overload]
    mtd nextS32()->s32
    {
        return cast #bit (s32) .nextU32()
    }

    // Returns a signed 64 bits random value
    #[Swag.Inline, Swag.Overload]
    mtd nextS64()->s64
    {
        return cast #bit (s64) .nextU64()
    }

    // Range is [0..1[, so 1.0 will never be returned
    #[Swag.Inline, Swag.Overload]
    mtd nextF32()->f32
    {
        var v = .nextU32()
        while v == Swag.U32.Max do
            v = .nextU32()
        return v / cast(f32) Swag.U32.Max
    }

    // Range is [0..1[, so 1.0 will never be returned
    #[Swag.Inline, Swag.Overload]
    mtd nextF64()->f64
    {
        var v = .nextU64()
        while v == Swag.U64.Max do
            v = .nextU64()
        return v / cast(f64) Swag.U64.Max
    }

    // Returns an unsigned random value in a given range
    // 'max' is excluded
    #[Swag.Overload]
    mtd nextU32(min, max: u32)->u32
    {
        Debug.assert(max > min, "[[max]] must be greater than [[min]]")
        let range = cast(u32) (max - min)
        let value = .nextU32() % range
        return min + value
    }

    // Returns an unsigned random value in a given range
    // 'max' is excluded
    #[Swag.Overload]
    mtd nextU64(min, max: u64)->u64
    {
        Debug.assert(max > min, "[[max]] must be greater than [[min]]")
        let range = cast(u64) (max - min)
        let value = .nextU64() % range
        return min + value
    }

    // Returns a signed random value in a given range
    // 'max' is excluded
    #[Swag.Overload]
    mtd nextS32(min, max: s32)->s32
    {
        Debug.assert(max > min, "[[max]] must be greater than [[min]]")
        let range = cast(u32) (max - min)
        let value = cast(s32) (.nextU32() % range)
        return min + value
    }

    // Returns a signed random value in a given range
    // 'max' is excluded
    #[Swag.Overload]
    mtd nextS64(min, max: s64)->s64
    {
        Debug.assert(max > min, "[[max]] must be greater than [[min]]")
        let range = cast(u64) (max - min)
        let value = cast(s64) (.nextU64() % range)
        return min + value
    }

    // Returns a float value in a given range
    // 'max' is excluded
    #[Swag.Overload]
    mtd nextF32(min, max: f32)->f32
    {
        Debug.assert(max > min, "[[max]] must be greater than [[min]]")
        let range = max - min
        let value = .nextF32()
        return min + (value * range)
    }

    // Returns a float value in a given range
    // 'max' is excluded
    #[Swag.Overload]
    mtd nextF64(min, max: f64)->f64
    {
        Debug.assert(max > min, "[[max]] must be greater than [[min]]")
        let range = max - min
        let value = .nextF64()
        return min + (value * range)
    }

    // Returns a random bool
    mtd nextBool()->bool
    {
        return .nextU32(0, 2) == 0 ? false : true
    }

    // Randomly change the order of a slice
    mtd(V) shuffle(slice: [..] V)
    {
        let countOf = @countof(slice)
        let dataOf  = @dataof(slice)
        for i in slice
        {
            let o = .nextU64(0, countOf)
            Core.swap(dataOf + i, dataOf + o)
        }
    }
}
