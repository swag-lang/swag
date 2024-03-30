using Core.Random

#test
{
    var rng: Rng
    rng.seedU32(0)
    @assert(cast,bit(u32) rng.nextU32() == 820856226)
    @assert(cast,bit(u64) rng.nextU64() == 10012380511238449408)
    @assert(cast,bit(u32) rng.nextS32() == 3169966213)
    @assert(cast,bit(u64) rng.nextS64() == 11050184656313024344)
    @assert(cast,bit(u32) rng.nextF32() == 1053447115)
    @assert(cast,bit(u64) rng.nextF64() == 4606720841166898557)
}
