#global namespace Hash

using Swag

// CRC-64 implementation as described in ECMA-182, Annex B.
public struct Crc64
{
    crc: u64
}

impl Crc64
{
    // Lookup table for CRC-64-ECMA.
    // This table is generated at compile time.
    var LookUpTable: [256] u64 = undefined

    // Generates the lookup LookUpTable for CRC-64-ECMA.
    #[Swag.Compiler]
    func generateLookUpTable()
    {
        const POLY: u64 = 0xC96C5795D7870F42

        for i in 256
        {
            var crc: u64 = i
            for j in 8
            {
                if crc & 1
                {
                    crc = (crc >> 1) ^ POLY
                }
                else
                {
                    crc >>= 1
                }
            }

            LookUpTable[i] = crc
        }
    }

    // Initialize the hashing sequence
    mtd init(seed: u64 = 0)
    {
        crc = seed
    }

    // Computes the CRC-64 hash of the provided data.
    #[ConstExpr, Swag.Safety("", false)]
    public mtd update(data: #null const [..] u8)
    {
        let length = @countof(data)
        if length == 0:
            return

        let bytes = cast(const ^u8) @dataof(data)
        var cur   = ~crc

        // Process 8 bytes at a time for better cache utilization
        let blocks = length >> 3 // length / 8

        // Unrolled loop for 8-byte blocks
        var i: u64 = 0
        while i < blocks
        {
            let offset = i << 3 // i * 8
            cur = LookUpTable[(cur ^ bytes[offset]) & 0xFF] ^ (cur >> 8)
            cur = LookUpTable[(cur ^ bytes[offset + 1]) & 0xFF] ^ (cur >> 8)
            cur = LookUpTable[(cur ^ bytes[offset + 2]) & 0xFF] ^ (cur >> 8)
            cur = LookUpTable[(cur ^ bytes[offset + 3]) & 0xFF] ^ (cur >> 8)
            cur = LookUpTable[(cur ^ bytes[offset + 4]) & 0xFF] ^ (cur >> 8)
            cur = LookUpTable[(cur ^ bytes[offset + 5]) & 0xFF] ^ (cur >> 8)
            cur = LookUpTable[(cur ^ bytes[offset + 6]) & 0xFF] ^ (cur >> 8)
            cur = LookUpTable[(cur ^ bytes[offset + 7]) & 0xFF] ^ (cur >> 8)
            i += 1
        }

        // Process remaining bytes
        let remaining_start = blocks << 3 // blocks * 8
        i = remaining_start
        while i < length
        {
            cur = LookUpTable[(cur ^ bytes[i]) & 0xFF] ^ (cur >> 8)
            i += 1
        }

        crc = ~cur
    }

    // Returns the hash value for the given buffer
    #[Swag.ConstExpr]
    public func compute(buffer: const [..] u8)->u64
    {
        var v: Crc64
        v.init()
        v.update(buffer)
        return v.crc
    }
}

// Initialize the lookup LookUpTable at compile time.
#run
{
    Crc64.generateLookUpTable()
}
