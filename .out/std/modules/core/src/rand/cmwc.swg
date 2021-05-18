// George Marsaglia's CMWC (complementary multiply with carry) generator.
#global namespace random
#global public

struct CMWC4096
{
    q: [4096] u32
    c: u32
    i: u32 = 4096 // special invalid value (> 4095) to force the seed on first get
}

impl CMWC4096
{
    func seedU32(using self, value: u32)
    {
        var rng: MWC
        rng.seedU32(value)
        loop idx: q
            q[idx] = rng.nextU32()
        i = 4095
        c = value % 809430660
    }

    #[swag.safety("overflow", false)]
    func nextU32(using self)->u32
    {
        const a = 18782'u64
        const r = 0xfffffffe

        // Be sure we have seeded
        if i > 4095 seedU32(362436)

	    i = (i + 1) & 4095
    	t := a * q[i] + c
	    c = cast(u32) (t >>,safe 32)
	    x := t + c
	    if x < c
            x, c += 1
        v := bitcast(u32) (r - x)
        q[i] = v
	    return v
    }
}
