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

// A 64-bit Globally Unique Identifier structure.
// Provides compact GUID generation with multiple entropy strategies for different use cases.
struct GUID64
{
    guid: u64 // 64-bit unique identifier value
}

impl GUID64
{
    // Compare two GUID64 instances for equality.
    // Returns 'true' if both GUIDs have identical values.
    mtd opEquals(other: GUID64)->bool
    {
        return .guid == other.guid
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
        .guid = 0
    }

    // Check if GUID is null/empty
    mtd isNull()->bool
    {
        return .guid == 0
    }

    // Convert GUID64 to hexadecimal string
    mtd toString(result: [..] u8)
    {
        Debug.assert(@countof(result) == 16)

        const Tbl = "0123456789ABCDEF"

        // Convert each nibble to hex, starting from most significant
        // Unroll the loop for better performance
        var idx = .guid
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
        .toString(result.toSlice())
        return result
    }

    // Parse GUID64 from hex string (with or without 0x prefix)
    func parse(str: string)->GUID64
    {
        var result:  retval = undefined
        var startIdx = 0

        // Skip "0x" prefix if present
        if @countof(str) >= 2 and str[0] == '0' and (str[1] == 'x' or str[1] == 'X') do
            startIdx = 2

        // Parse hex digits
        var value: u64 = 0
        for i in Math.min(16, @countof(str) - startIdx)
        {
            let c      = str[startIdx + i]
            var digit: u64 = 0

            if c >= '0' and c <= '9' do
                digit = cast(u64) (c - '0')
            elif c >= 'A' and c <= 'F' do
                digit = cast(u64) (c - 'A' + 10)
            elif c >= 'a' and c <= 'f' do
                digit = cast(u64) (c - 'a' + 10)
            else do
                break // Invalid character, stop parsing

            value = (value << 4) | digit
        }

        result.guid = value
        return result
    }
}

// String conversion interface implementation for GUID64.
// Enables automatic string conversion in formatting contexts.
impl StrConv.IConvert for GUID64
{
    // Convert the GUID to its hexadecimal string representation.
    // Produces a 16-character uppercase hex string without separators.
    mtd impl convert(buf: *ConcatBuffer, convFmt: StrConv.ConvertFormat, strFormat: string)
    {
        buf.addBytes(.getString())
    }
}

// Hash interface implementation for GUID64.
// Enables use of GUID64 as keys in hash tables and sets.
impl Hash.IHash32 for GUID64
{
    // Compute a 32-bit hash value for the GUID.
    // Uses CRC32 algorithm on the 8-byte GUID data for consistent hashing.
    mtd const impl compute()->u32
    {
        return Hash.Crc32.compute(@mkslice(cast(const [*] u8) &.guid, #sizeof(u64)))
    }
}
