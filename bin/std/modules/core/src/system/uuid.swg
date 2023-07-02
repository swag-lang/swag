#global public

#[Swag.Tls]
{
    private var g_Rng:      Random.Mt64
    private var g_RngInit:  bool
}

struct UUID
{
    using uuid: union
    {
        val8:  [16] u8
        val64: [2]  u64
    }
}

#[Swag.Inline]
private func initRng()
{
    if !g_RngInit
    {
        var seed: [3] u64 = undefined
        seed[0] = Time.nowMilliseconds()
        seed[1] = Time.nowMicroseconds()
        seed[2] = Hardware.getMacAddress()
        g_Rng.seedU64(seed)
        g_RngInit = true
    }
}

impl UUID
{
    mtd opEquals(other: Self)->bool
    {
        return val64[0] == other.val64[0] and val64[1] == other.val64[1]
    }

    // Get a version 4 UUID (random)
    func getRandom()->UUID
    {
        initRng()
        var result: retval = undefined
        result.val64[0] = g_Rng.nextU64()
        result.val64[1] = g_Rng.nextU64()
        result.val8[6] = (result.val8[6] & 0x0F) | 0x40 // Version 4
        result.val8[8] = (result.val8[8] & 0x3F) | 0x80 // Variant is 10
        return result
    }

    // Clear id
    mtd clear()
    {
        val64[0], val64[1] = 0
    }

    // Convert uuid to a string of the form xxxxxxxx-xxxx-Mxxx-Nxxx-xxxxxxxxxxxx
    #[Swag.Safety("boundcheck", false)]
    mtd toString(result: [..] u8)
    {
        Debug.assert(@countof(result) == 36)

        const Tbl = "0123456789abcdef"

        result[0] = Tbl[(val8[0] & 0xF)]
        result[1] = Tbl[(val8[0] >> 4)]
        result[2] = Tbl[(val8[1] & 0xF)]
        result[3] = Tbl[(val8[1] >> 4)]
        result[4] = Tbl[(val8[2] & 0xF)]
        result[5] = Tbl[(val8[2] >> 4)]
        result[6] = Tbl[(val8[3] & 0xF)]
        result[7] = Tbl[(val8[3] >> 4)]
        result[8] = "-"'u8

        result[9] = Tbl[(val8[4] & 0xF)]
        result[10] = Tbl[(val8[4] >> 4)]
        result[11] = Tbl[(val8[5] & 0xF)]
        result[12] = Tbl[(val8[5] >> 4)]
        result[13] = "-"'u8

        result[14] = Tbl[(val8[6] & 0xF)]
        result[15] = Tbl[(val8[6] >> 4)]
        result[16] = Tbl[(val8[7] & 0xF)]
        result[17] = Tbl[(val8[7] >> 4)]
        result[18] = "-"'u8

        result[19] = Tbl[(val8[8] & 0xF)]
        result[20] = Tbl[(val8[8] >> 4)]
        result[21] = Tbl[(val8[9] & 0xF)]
        result[22] = Tbl[(val8[9] >> 4)]
        result[23] = "-"'u8

        result[24] = Tbl[(val8[10] & 0xF)]
        result[25] = Tbl[(val8[10] >> 4)]
        result[26] = Tbl[(val8[11] & 0xF)]
        result[27] = Tbl[(val8[11] >> 4)]
        result[28] = Tbl[(val8[12] & 0xF)]
        result[29] = Tbl[(val8[12] >> 4)]
        result[30] = Tbl[(val8[13] & 0xF)]
        result[31] = Tbl[(val8[13] >> 4)]
        result[32] = Tbl[(val8[14] & 0xF)]
        result[33] = Tbl[(val8[14] >> 4)]
        result[34] = Tbl[(val8[15] & 0xF)]
        result[35] = Tbl[(val8[15] >> 4)]
    }

    // Convert uuid to a string
    mtd toString()->String
    {
        var result: retval
        result.reserve(36)
        result.length = 36
        toString(result.buffer[0..35])
        return result
    }
}

impl StrConv.IConvert for UUID
{
    mtd impl convert(buf: *ConcatBuffer, fmt: const *StrConv.ConvertFormat, strFormat: string)
    {
        buf.addBytes(toString())
    }
}

impl Hash.IHash32 for UUID
{
    mtd const impl compute()->u32
    {
        return Hash.Crc32.compute(@mkslice(&val8[0], 16))
    }
}