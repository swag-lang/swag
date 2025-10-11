#global namespace Random
#global public

// George Marsaglia's CMWC (Complementary Multiply With Carry) 4096 random number generator.
struct CMWC4096
{
    q:     [4096] u32     // State array of 4096 32-bit unsigned integers
    c:     u32            // Carry value
    i:     u32 = 4096     // Current index into the state array (initialized to an invalid value to trigger seeding)
}

impl CMWC4096
{
    // Seeds the generator with a 32-bit unsigned value.
    mtd seedU32(value: u32)
    {
        var rng: MWC
        rng.seedU32(value)
        for idx in .q do
            .q[idx] = rng.nextU32()
        .i = 4095
        .c = value % 809430660
    }

    // Generates the next 32-bit unsigned random number.
    #[Swag.CanOverflow(true)]
    mtd nextU32()->u32
    {
        const a = 18782'u64
        const r = 0xfffffffe

        // Be sure we have seeded
        if .i > 4095 do
            .seedU32(362436)

        .i = (.i + 1) & 4095
        let t = a * .q[.i] + .c
        .c = cast(u32) (t >> 32)
        var x = t + .c
        if x < .c do
            x, .c += 1
        let v = cast #bit (u32) (r - x)
        .q[.i] = v
        return v
    }
}
