#global namespace Hash
#global #[Swag.CanOverflow(true)]

// A SHA-1 cryptographic hash function implementation.
// Provides methods to compute 160-bit SHA-1 hash values from arbitrary data streams.
// Supports incremental hashing through 'init', 'update', and 'final' operations.
public struct Sha1
{
    data:        [64] u8     // Internal data buffer for processing 512-bit blocks
    state:       [5] u32     // Five 32-bit hash state values (A, B, C, D, E)
    datalen:     u32         // Current number of bytes in the data buffer
    bitlen:      u64         // Total number of bits processed so far
}

impl Sha1
{
    #[Swag.Inline]
    {
        func rotLeft(a, b: u32) => ((a << b) | (a >> (32 - b)))
        func f1(x, y, z: u32)   => ((x & y) | (~x & z))
        func f2(x, y, z: u32)   => (x ^ y ^ z)
        func f3(x, y, z: u32)   => ((x & y) | (x & z) | (y & z))
        func f4(x, y, z: u32)   => (x ^ y ^ z)
    }

    // Process a single 512-bit block of data.
    // Performs the main SHA-1 compression function on the provided 'buf'.
    mtd transform(buf: const [*] u8)
    {
        var w: [80] u32

        var i, j = 0'u32
        while i < 16
        {
            w[i] = (cast(u32) buf[j] << 24) | (cast(u32) buf[j + 1] << 16) | (cast(u32) buf[j + 2] << 8) | buf[j + 3]
            i += 1
            j += 4
        }

        while i < 80
        {
            w[i] = rotLeft(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1)
            i += 1
        }

        var a = .state[0]
        var b = .state[1]
        var c = .state[2]
        var d = .state[3]
        var e = .state[4]

        i = 0
        while i < 80
        {
            var f, k: u32
            if i < 20
            {
                f = f1(b, c, d)
                k = 0x5a827999
            }
            elif i < 40
            {
                f = f2(b, c, d)
                k = 0x6ed9eba1
            }
            elif i < 60
            {
                f = f3(b, c, d)
                k = 0x8f1bbcdc
            }
            else
            {
                f = f4(b, c, d)
                k = 0xca62c1d6
            }

            let temp = rotLeft(a, 5) + f + e + k + w[i]
            e = d
            d = c
            c = rotLeft(b, 30)
            b = a
            a = temp
            i += 1
        }

        .state[0] += a
        .state[1] += b
        .state[2] += c
        .state[3] += d
        .state[4] += e
    }
}

public impl Sha1
{
    alias Digest = [5] u32 // SHA-1 digest result as five 32-bit words

    // Finalize the computation and produce the final hash digest.
    // Applies padding and length encoding, then stores the result in 'result'.
    mtd final(result: Digest)
    {
        var i = .datalen
        if .datalen < 56
        {
            .data[i] = 0x80
            i += 1
            while i < 56
            {
                .data[i] = 0x00
                i += 1
            }
        }
        else
        {
            .data[i] = 0x80
            i += 1
            while i < 64
            {
                .data[i] = 0x00
                i += 1
            }

            .transform(.data)

            i = 0
            while i < 56
            {
                .data[i] = 0x00
                i += 1
            }
        }

        .bitlen += .datalen * 8
        .data[63] = cast #bit (u8) .bitlen
        .data[62] = cast #bit (u8) (.bitlen >> 8)
        .data[61] = cast #bit (u8) (.bitlen >> 16)
        .data[60] = cast #bit (u8) (.bitlen >> 24)
        .data[59] = cast #bit (u8) (.bitlen >> 32)
        .data[58] = cast #bit (u8) (.bitlen >> 40)
        .data[57] = cast #bit (u8) (.bitlen >> 48)
        .data[56] = cast #bit (u8) (.bitlen >> 56)
        .transform(.data)

        Memory.copy(result, &.state[0], #sizeof(.state))
    }

    // Initialize the hashing sequence with standard SHA-1 constants.
    // Sets up the initial state values and resets all counters to zero.
    mtd init()
    {
        @init(me, 1)
        .state[0] = 0x67452301
        .state[1] = 0xefcdab89
        .state[2] = 0x98badcfe
        .state[3] = 0x10325476
        .state[4] = 0xc3d2e1f0
    }

    // Update the hash value with 'buffer' content.
    // Processes the input data in 512-bit blocks, buffering incomplete blocks for later processing.
    mtd update(buffer: const [..] u8)
    {
        Debug.assert(@countof(buffer) <= Swag.U32.Max)
        let buf = @dataof(buffer)
        let len = cast(u32) @countof(buffer)

        for i in len
        {
            .data[.datalen] = buf[i]
            .datalen += 1
            if .datalen == 64
            {
                .transform(.data)
                .bitlen += 512
                .datalen = 0
            }
        }
    }

    // Returns the SHA-1 hash value for the given buffer.
    // Convenience function that performs the complete hash operation in a single call.
    func compute(buffer: const [..] u8)->Digest
    {
        var v: Sha1
        v.init()
        v.update(buffer)

        var result: retval
        v.final(result)
        return result
    }
}
