#global public

// Thread-local storage for random number generation and GUID counter.
#[Swag.Tls]
{
    private var g_Rng: Random.Mt64 // Mersenne Twister 64-bit random number generator
    private var g_RngInit: bool // Flag indicating whether the RNG has been initialized
    private var g_Counter: u32 // Thread-local counter for timestamp-based GUIDs
}

// Initialize the random number generator with high-entropy seed.
// Uses multiple time-based and hardware-based entropy sources for better randomness.
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

// A 128-bit Globally Unique Identifier structure.
// Provides RFC 4122 compliant UUID generation and manipulation.
struct GUID128
{
    using guid: union
    {
        val8:      [16] u8     // 16-byte array representation for byte-level operations
        val64:     [2] u64     // 2 64-bit values for efficient comparison and storage
    }
}

impl GUID128
{
    // Compare two GUID128 instances for equality.
    // Returns 'true' if both GUIDs have identical values.
    mtd opEquals(other: GUID128)->bool
    {
        return .val64[0] == other.val64[0] and .val64[1] == other.val64[1]
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
        .val64[0], .val64[1] = 0
    }

    // Convert guid to a string of the form xxxxxxxx-xxxx-Mxxx-Nxxx-xxxxxxxxxxxx
    #[Swag.Safety(.All, .BoundCheck, false), Swag.Overload]
    mtd toString(result: [..] u8)
    {
        Debug.assert(@countof(result) == 36)

        const Tbl = "0123456789abcdef"

        result[0] = Tbl[(.val8[0] & 0xF)]
        result[1] = Tbl[(.val8[0] >> 4)]
        result[2] = Tbl[(.val8[1] & 0xF)]
        result[3] = Tbl[(.val8[1] >> 4)]
        result[4] = Tbl[(.val8[2] & 0xF)]
        result[5] = Tbl[(.val8[2] >> 4)]
        result[6] = Tbl[(.val8[3] & 0xF)]
        result[7] = Tbl[(.val8[3] >> 4)]
        result[8] = '-'

        result[9]  = Tbl[(.val8[4] & 0xF)]
        result[10] = Tbl[(.val8[4] >> 4)]
        result[11] = Tbl[(.val8[5] & 0xF)]
        result[12] = Tbl[(.val8[5] >> 4)]
        result[13] = '-'

        result[14] = Tbl[(.val8[6] & 0xF)]
        result[15] = Tbl[(.val8[6] >> 4)]
        result[16] = Tbl[(.val8[7] & 0xF)]
        result[17] = Tbl[(.val8[7] >> 4)]
        result[18] = '-'

        result[19] = Tbl[(.val8[8] & 0xF)]
        result[20] = Tbl[(.val8[8] >> 4)]
        result[21] = Tbl[(.val8[9] & 0xF)]
        result[22] = Tbl[(.val8[9] >> 4)]
        result[23] = '-'

        result[24] = Tbl[(.val8[10] & 0xF)]
        result[25] = Tbl[(.val8[10] >> 4)]
        result[26] = Tbl[(.val8[11] & 0xF)]
        result[27] = Tbl[(.val8[11] >> 4)]
        result[28] = Tbl[(.val8[12] & 0xF)]
        result[29] = Tbl[(.val8[12] >> 4)]
        result[30] = Tbl[(.val8[13] & 0xF)]
        result[31] = Tbl[(.val8[13] >> 4)]
        result[32] = Tbl[(.val8[14] & 0xF)]
        result[33] = Tbl[(.val8[14] >> 4)]
        result[34] = Tbl[(.val8[15] & 0xF)]
        result[35] = Tbl[(.val8[15] >> 4)]
    }

    // Convert guid to a string
    #[Swag.Overload]
    mtd getString()->String
    {
        var result: retval
        result.reserve(36)
        result.length = 36
        .toString(result.toSlice())
        return result
    }
}

// String conversion interface implementation for GUID128.
// Enables automatic string conversion in formatting contexts.
impl StrConv.IConvert for GUID128
{
    // Convert the GUID to its string representation for display.
    // Uses the standard UUID format with hyphens.
    mtd impl convert(buf: *ConcatBuffer, convFmt: StrConv.ConvertFormat, strFormat: string)
    {
        buf.addBytes(.getString())
    }
}

// Hash interface implementation for GUID128.
// Enables use of GUID128 as keys in hash tables and sets.
impl Hash.IHash32 for GUID128
{
    // Compute a 32-bit hash value for the GUID.
    // Uses CRC32 algorithm on the entire 16-byte GUID data.
    mtd const impl compute()->u32
    {
        return Hash.Crc32.compute(@mkslice(&.val8[0], 16))
    }
}
