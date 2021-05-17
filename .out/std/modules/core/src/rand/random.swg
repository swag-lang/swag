#global namespace random

public struct(T = Mwc) Rng
{
    rng: T
}

public impl Rng
{
    func seedU32(using self, value: u32)
    {
        return rng.seedU32(value)
    }

    func nextS32(using self)->s32
    {
        return bitcast(s32) rng.nextU32()
    }

    func nextS64(using self)->s64
    {
        return bitcast(s64) nextU64(self)
    }

    func nextU32(using self)->u32
    {
        return rng.nextU32()
    }

    func nextU64(using self)->u64
    {
        return (cast(u64) rng.nextU32() << 32) | rng.nextU32()
    }

    func nextF32(using self)->f32
    {
        return rng.nextU32() / cast(f32) swag.U32.Max
    }

    func nextF64(using self)->f64
    {
        return nextU64(self) / cast(f64) swag.U64.Max
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
