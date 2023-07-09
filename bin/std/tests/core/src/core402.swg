using Core, Random

#test
{
    var rng: Mt64
    rng.seedU64(cast(u64) Time.nowMilliseconds())
    loop 10
    {
        var x = rng.nextU64()
        //@print(x, "\n")
    }

    var s: [4] u64
    s[0] = cast(u64) 1
    s[1] = cast(u64) 2
    s[2] = cast(u64) 3
    s[3] = cast(u64) 4
    rng.seedU64(s)
    loop 10
    {
        var x = rng.nextU64()
        //@print(x, "\n")
    }

    var rng1: Rng'Mt64
    rng1.seedU32(Time.nowMilliseconds())
    var x = rng1.nextU32()
    //@print(x)
}