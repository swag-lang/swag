using Core, Random

#test
{
    var rng: Mt19937
    rng.seedU64(cast(u64) Time.nowMilliseconds())
    loop 10
    {
        x := rng.nextU64()
        //@print(x, "\n")
    }

    var rng1: Rng'Mt19937
    rng1.seedU32(Time.nowMilliseconds())
    x := rng1.nextU32()
    //@print(x)
}