using Core.Random

#test
{
    var rng: Rng
    rng.seedU32(0)
    @assert(cast(u32) #bit rng.nextU32() == 820856226)
    @assert(cast(u64) #bit rng.nextU64() == 10012380511238449408)
    @assert(cast(u32) #bit rng.nextS32() == 3169966213)
    @assert(cast(u64) #bit rng.nextS64() == 11050184656313024344)
    @assert(cast(u32) #bit rng.nextF32() == 1053447115)
    @assert(cast(u64) #bit rng.nextF64() == 4606720841166898557)
}
