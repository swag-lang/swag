// George Marsaglia's MWC (multiply with carry) generator.
#global namespace Random
#global public

struct MWC
{
    w: u32 = 521288629
    z: u32 = 362436069
}

impl MWC
{
    func seedU32(using self, value: u32)
    {
        w = value orelse 521288629
        z = 362436069
    }

    func seedU32(using self, value1, value2: u32)
    {
        w = value1 orelse 521288629
        z = value2 orelse 362436069
    }

    #[Swag.Safety("overflow", false)]
    func nextU32(using self)->u32
    {
        z = 36969 * (z & 0xffff) + (z >> 16)
        w = 18000 * (w & 0xffff) + (w >> 16)
        return (z << 16) + w
    }
}
