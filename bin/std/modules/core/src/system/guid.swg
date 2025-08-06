#global public

#[Swag.Tls]
{
    internal var g_Rng: Random.Mt64
    internal var g_RngInit: bool
    internal var g_Counter: u32
}

#[Swag.Inline]
internal func initRng()
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

struct GUID128
{
    using guid: union
    {
        val8:      [16] u8
        val64:     [2] u64
    }
}

impl GUID128
{
    mtd opEquals(other: Self)->bool
    {
        return val64[0] == other.val64[0] and val64[1] == other.val64[1]
    }

    // Get a version 4 GUID128 (random)
    func getRandom()->GUID128
    {
        initRng()
        var result: retval = undefined
        result.val64[0] = g_Rng.nextU64()
        result.val64[1] = g_Rng.nextU64()
        result.val8[6]  = (result.val8[6] & 0x0F) | 0x40 // Version 4
        result.val8[8]  = (result.val8[8] & 0x3F) | 0x80 // Variant is 10
        return result
    }

    // Clear id
    mtd clear()
    {
        val64[0], val64[1] = 0
    }

    // Convert guid to a string of the form xxxxxxxx-xxxx-Mxxx-Nxxx-xxxxxxxxxxxx
    #[Swag.Safety("boundcheck", false), Swag.Overload]
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
        result[8] = '-'

        result[9]  = Tbl[(val8[4] & 0xF)]
        result[10] = Tbl[(val8[4] >> 4)]
        result[11] = Tbl[(val8[5] & 0xF)]
        result[12] = Tbl[(val8[5] >> 4)]
        result[13] = '-'

        result[14] = Tbl[(val8[6] & 0xF)]
        result[15] = Tbl[(val8[6] >> 4)]
        result[16] = Tbl[(val8[7] & 0xF)]
        result[17] = Tbl[(val8[7] >> 4)]
        result[18] = '-'

        result[19] = Tbl[(val8[8] & 0xF)]
        result[20] = Tbl[(val8[8] >> 4)]
        result[21] = Tbl[(val8[9] & 0xF)]
        result[22] = Tbl[(val8[9] >> 4)]
        result[23] = '-'

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

    // Convert guid to a string
    #[Swag.Overload]
    mtd getString()->String
    {
        var result: retval
        result.reserve(36)
        result.length = 36
        toString(result.toSlice())
        return result
    }
}

impl StrConv.IConvert for GUID128
{
    mtd impl convert(buf: *ConcatBuffer, convFmt: StrConv.ConvertFormat, strFormat: string)
    {
        buf.addBytes(getString())
    }
}

impl Hash.IHash32 for GUID128
{
    mtd const impl compute()->u32
    {
        return Hash.Crc32.compute(@mkslice(&val8[0], 16))
    }
}

struct GUID64
{
    guid: u64
}

impl GUID64
{
    mtd opEquals(other: Self)->bool
    {
        return guid == other.guid
    }

    // Get a timestamp-based GUID64 with minimal collision risk
    // Structure: 32-bit timestamp + 16-bit microseconds + 12-bit counter + 4-bit random
    func getTimestamp()->GUID64
    {
        initRng()

        var result: retval = undefined
        let nowMs   = Time.nowMilliseconds()
        let nowUs   = Time.nowMicroseconds()

        // Extract components
        let timestamp = cast(u32) (nowMs / 1000)               // 32-bit Unix timestamp
        let microsec  = cast(u16) (nowUs % 1000000 / 1000)     // 16-bit milliseconds component

        // Thread-local counter with overflow protection
        g_Counter = (g_Counter + 1) & 0xFFF // 12-bit counter (0-4095)

        let random4 = cast(u8) (g_Rng.nextU64() & 0xF) // 4-bit random

        // Combine all components
        result.guid = 0
        result.guid |= cast(u64) timestamp << 32 // Upper 32 bits: timestamp
        result.guid |= cast(u64) microsec << 16 // Next 16 bits: milliseconds
        result.guid |= cast(u64) g_Counter << 4 // Next 12 bits: counter
        result.guid |= cast(u64) random4 // Lower 4 bits: random

        return result
    }

    // Get a fully random GUID64 using MT64
    func getRandom()->GUID64
    {
        initRng()

        var result: retval = undefined
        result.guid = g_Rng.nextU64()
        return result
    }

    // Get a high-entropy GUID64 using multiple entropy sources
    func getHighEntropy()->GUID64
    {
        initRng()

        var result: retval = undefined
        let nowMs   = Time.nowMilliseconds()
        let nowUs   = Time.nowMicroseconds()
        let pid     = cast(u64) Env.Process.getProcessId()
        let tid     = cast(u64) Env.Process.getThreadId()

        // Mix multiple entropy sources using XOR
        result.guid = g_Rng.nextU64()
        result.guid ^= nowMs
        result.guid ^= (nowUs << 16)
        result.guid ^= (pid << 32)
        result.guid ^= (tid << 8)

        return result
    }

    // Clear GUID
    mtd clear()
    {
        guid = 0
    }

    // Check if GUID is null/empty
    mtd isNull()->bool
    {
        return guid == 0
    }

    // Convert GUID64 to hexadecimal string
    mtd toString(result: [..] u8)
    {
        Debug.assert(@countof(result) == 16)

        const Tbl  = "0123456789ABCDEF"

        // Convert each nibble to hex, starting from most significant
        // Unroll the loop for better performance
        var idx = guid
        result[15] = Tbl[idx & 0xF]; idx >>= 4
        result[14] = Tbl[idx & 0xF]; idx >>= 4
        result[13] = Tbl[idx & 0xF]; idx >>= 4
        result[12] = Tbl[idx & 0xF]; idx >>= 4
        result[11] = Tbl[idx & 0xF]; idx >>= 4
        result[10] = Tbl[idx & 0xF]; idx >>= 4
        result[9] = Tbl[idx & 0xF]; idx >>= 4
        result[8] = Tbl[idx & 0xF]; idx >>= 4
        result[7] = Tbl[idx & 0xF]; idx >>= 4
        result[6] = Tbl[idx & 0xF]; idx >>= 4
        result[5] = Tbl[idx & 0xF]; idx >>= 4
        result[4] = Tbl[idx & 0xF]; idx >>= 4
        result[3] = Tbl[idx & 0xF]; idx >>= 4
        result[2] = Tbl[idx & 0xF]; idx >>= 4
        result[1] = Tbl[idx & 0xF]; idx >>= 4
        result[0] = Tbl[idx & 0xF]
    }

    // Convert guid to a string
    #[Swag.Overload]
    mtd getString()->String
    {
        var result: retval
        result.reserve(16)
        result.length = 16
        toString(result.toSlice())
        return result
    }

    // Parse GUID64 from hex string (with or without 0x prefix)
    func parse(str: string)->GUID64
    {
        var result:  retval = undefined
        var startIdx = 0

        // Skip "0x" prefix if present
        if @countof(str) >= 2 and str[0] == '0' and (str[1] == 'x' or str[1] == 'X'):
            startIdx = 2

        // Parse hex digits
        var value: u64 = 0
        for i in Math.min(16, @countof(str) - startIdx)
        {
            let c      = str[startIdx + i]
            var digit: u64 = 0

            if c >= '0' and c <= '9':
                digit = cast(u64) (c - '0')
            elif c >= 'A' and c <= 'F':
                digit = cast(u64) (c - 'A' + 10)
            elif c >= 'a' and c <= 'f':
                digit = cast(u64) (c - 'a' + 10)
            else:
                break // Invalid character, stop parsing

            value = (value << 4) | digit
        }

        result.guid = value
        return result
    }
}

impl StrConv.IConvert for GUID64
{
    mtd impl convert(buf: *ConcatBuffer, convFmt: StrConv.ConvertFormat, strFormat: string)
    {
        buf.addBytes(getString())
    }
}

impl Hash.IHash32 for GUID64
{
    mtd const impl compute()->u32
    {
        return Hash.Crc32.compute(@mkslice(cast(const ^u8) &guid, #sizeof(u64)))
    }
}
