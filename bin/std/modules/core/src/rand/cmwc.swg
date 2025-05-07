// George Marsaglia's CMWC (complementary multiply with carry) generator.
#global namespace Random
#global public

struct CMWC4096
{
    q:     [4096] u32
    c:     u32
    i:     u32 = 4096     // special invalid value (> 4095) to force the seed on first get
}

impl CMWC4096
{
    mtd seedU32(value: u32)
    {
        var rng: MWC
        rng.seedU32(value)
        for idx in q:
            q[idx] = rng.nextU32()
        i = 4095
        c = value % 809430660
    }

    #[Swag.CanOverflow(true)]
    mtd nextU32()->u32
    {
        const a = 18782'u64
        const r = 0xfffffffe

        // Be sure we have seeded
        if i > 4095:
            seedU32(362436)

        i = (i + 1) & 4095
        let t = a * q[i] + c
        c = cast(u32) (t >> 32)
        var x = t + c
        if x < c:
            x, c += 1
        let v = cast<bit>(u32) (r - x)
        q[i] = v
        return v
    }
}
