// George Marsaglia's MWC (multiply with carry) generator.
#global marked
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
        self.w = value orelse 521288629
        self.z = 362436069
    }

    #[Swag.Overload]
    mtd seedU32(value1, value2: u32)
    {
        self.w = value1 orelse 521288629
        self.z = value2 orelse 362436069
    }

    #[Swag.CanOverflow(true)]
    mtd nextU32()->u32
    {
        self.z = 36969 * (self.z & 0xffff) + (self.z >> 16)
        self.w = 18000 * (self.w & 0xffff) + (self.w >> 16)
        return (self.z << 16) + self.w
    }
}
