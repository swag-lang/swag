// George Marsaglia's MWC (multiply with carry) generator.
#global namespace Random
#global public

struct MWC
{
    w:     u32 = 521288629
    z:     u32 = 362436069
}

impl MWC
{
    #[Swag.Overload]
    mtd seedU32(value: u32)
    {
        me.w = value orelse 521288629
        me.z = 362436069
    }

    #[Swag.Overload]
    mtd seedU32(value1, value2: u32)
    {
        me.w = value1 orelse 521288629
        me.z = value2 orelse 362436069
    }

    #[Swag.CanOverflow(true)]
    mtd nextU32()->u32
    {
        me.z = 36969 * (me.z & 0xffff) + (me.z >> 16)
        me.w = 18000 * (me.w & 0xffff) + (me.w >> 16)
        return (me.z << 16) + me.w
    }
}
