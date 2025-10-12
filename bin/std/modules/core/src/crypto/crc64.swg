#global namespace Hash

using Swag

// CRC-64 implementation as described in ECMA-182, Annex B.
public struct Crc64
{
    crc: u64
}

public enum Crc64Kind
{
    ECMA182
    ISO
    ISOXZ
}

impl Crc64
{
    var LookUpTableECMA: [256] u64 = undefined
    var LookUpTableISO:  [256] u64 = undefined

    // Generates the lookup LookUpTable for CRC-64-ECMA.
    #[Swag.Compiler]
    func generateLookUpTableECMA()
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

            LookUpTableECMA[i] = crc
        }
    }

    // Generates the lookup LookUpTable for ISO.
    #[Swag.Compiler]
    func generateLookUpTableISO()
    {
        const POLY: u64 = 0xD800000000000000

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

            LookUpTableISO[i] = crc
        }
    }

    // Initialize the hashing sequence
    mtd init(kind: Crc64Kind)
    {
        #[Swag.Complete]
        switch kind
        {
        case ECMA182, ISO:
            .crc = 0
        case ISOXZ:
            .crc = 0xFFFFFFFF_FFFFFFFF
        }
    }

    // Computes the CRC-64 hash of the provided data.
    #[ConstExpr, Swag.Safety(.All, .All, false)]
    public mtd update(data: #null const [..] u8, kind: Crc64Kind)
    {
        let length = @countof(data)
        if length == 0 do
            return

        let bytes = cast(const [*] u8) @dataof(data)
        var cur   = ~.crc

        // Process 8 bytes at a time for better cache utilization
        let blocks = length >> 3

        var table: [*] u64
        #[Swag.Complete]
        switch kind
        {
        case ECMA182, ISOXZ:
            table = LookUpTableECMA
        case ISO:
            table = LookUpTableISO
        }

        // Unrolled loop for 8-byte blocks
        var i: u64 = 0
        while i < blocks
        {
            let offset = i << 3
            cur = table[(cur ^ bytes[offset]) & 0xFF] ^ (cur >> 8)
            cur = table[(cur ^ bytes[offset + 1]) & 0xFF] ^ (cur >> 8)
            cur = table[(cur ^ bytes[offset + 2]) & 0xFF] ^ (cur >> 8)
            cur = table[(cur ^ bytes[offset + 3]) & 0xFF] ^ (cur >> 8)
            cur = table[(cur ^ bytes[offset + 4]) & 0xFF] ^ (cur >> 8)
            cur = table[(cur ^ bytes[offset + 5]) & 0xFF] ^ (cur >> 8)
            cur = table[(cur ^ bytes[offset + 6]) & 0xFF] ^ (cur >> 8)
            cur = table[(cur ^ bytes[offset + 7]) & 0xFF] ^ (cur >> 8)
            i += 1
        }

        // Process remaining bytes
        let remaining_start = blocks << 3
        i = remaining_start
        while i < length
        {
            cur = table[(cur ^ bytes[i]) & 0xFF] ^ (cur >> 8)
            i += 1
        }

        .crc = ~cur
    }

    // Returns the hash value for the given buffer
    #[Swag.ConstExpr]
    public func compute(buffer: const [..] u8, kind = Crc64Kind.ECMA182)->u64
    {
        var v: Crc64
        v.init(kind)
        v.update(buffer, kind)
        return v.crc
    }
}

// Initialize the lookup LookUpTableECMA at compile time.
#run
{
    Crc64.generateLookUpTableECMA()
    Crc64.generateLookUpTableISO()
}
