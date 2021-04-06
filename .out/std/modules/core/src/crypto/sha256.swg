#global namespace hash
#global #[swag.safety("of", false)]

public struct Sha256
{
}

public impl Sha256
{
    #[swag.inline]
    private func rotr32(x, n: u32) => (x >> n) | (x << (32 - n))

    // Returns the sha256 value for the given buffer
    func compute(buffer: const [..] u8)->[8] u32
    {
        var result: retval

        result[0] = 0x6A09E667
        result[1] = 0xBB67AE85
        result[2] = 0x3C6EF372
        result[3] = 0xA54FF53A
        result[4] = 0x510E527F
        result[5] = 0x9B05688C
        result[6] = 0x1F83D9AB
        result[7] = 0x5BE0CD19

        const k: [64] u32 = @[
            0x428A2F98, 0x71374491, 0xB5C0FBCF, 0xE9B5DBA5, 0x3956C25B, 0x59F111F1, 0x923F82A4, 0xAB1C5ED5,
            0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3, 0x72BE5D74, 0x80DEB1FE, 0x9BDC06A7, 0xC19BF174,
            0xE49B69C1, 0xEFBE4786, 0x0FC19DC6, 0x240CA1CC, 0x2DE92C6F, 0x4A7484AA, 0x5CB0A9DC, 0x76F988DA,
            0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7, 0xC6E00BF3, 0xD5A79147, 0x06CA6351, 0x14292967,
            0x27B70A85, 0x2E1B2138, 0x4D2C6DFC, 0x53380D13, 0x650A7354, 0x766A0ABB, 0x81C2C92E, 0x92722C85,
            0xA2BFE8A1, 0xA81A664B, 0xC24B8B70, 0xC76C51A3, 0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070,
            0x19A4C116, 0x1E376C08, 0x2748774C, 0x34B0BCB5, 0x391C0CB3, 0x4ED8AA4A, 0x5B9CCA4F, 0x682E6FF3,
            0x748F82EE, 0x78A5636F, 0x84C87814, 0x8CC70208, 0x90BEFFFA, 0xA4506CEB, 0xBEF9A3F7, 0xC67178F2
        ]

        debug.assert(@countof(buffer) <= swag.MaxU32)
        buf := @dataof(buffer)
        len := cast(u32) @countof(buffer)

        messageBits  := len * 8
        adjustedBits := messageBits + 1 + 64
        paddedBits   := adjustedBits + (512 - (adjustedBits % 512))
        paddedLength := paddedBits / 8

        rounds := paddedLength / 64
        appendedOne := false

        loop round: rounds
        {
            var w: [64] u32
            w8 := cast(*u8) &w[0]

            offset := round * 64
            if offset < len
                memory.copy(&w[0], buf + offset, cast(uint) math.min(len - offset, 64))

            if !appendedOne && len < offset + 64
            {
                if offset < len
                    w8[len - offset] = 0x80
                else
                    w[0] = 0x80000000
                appendedOne = true
            }

            if round == rounds - 1
            {
                w8[63] = cast(u8) (messageBits & 0xFF)
                w8[62] = cast(u8) ((messageBits >> 8) & 0xFF)
                w8[61] = cast(u8) ((messageBits >> 16) & 0xFF)
                w8[60] = cast(u8) ((messageBits >> 24) & 0xFF)
            }

            for i := 16; i < 64; i += 1
            {
                s0 := rotr32(w[i - 15], 7) ^ rotr32(w[i - 15], 18) ^ (w[i - 15] >> 3)
                s1 := rotr32(w[i - 2], 17) ^ rotr32(w[i - 2], 19) ^ (w[i - 2] >> 10)
                w[i] = w[i - 16] + s0 + w[i - 7] + s1
            }

            a := result[0]
            b := result[1]
            c := result[2]
            d := result[3]
            e := result[4]
            f := result[5]
            g := result[6]
            h := result[7]

            loop i: 64
            {
                s1   := rotr32(e, 6) ^ rotr32(e, 11) ^ rotr32(e, 25)
                ch   := (e & f) ^ ((~e) & g)
                tmp1 := h + s1 + ch + k[i] + w[i]
                s0   := rotr32(a, 2) ^ rotr32(a, 13) ^ rotr32(a, 22)
                maj  := (a & b) ^ (a & c) ^ (b & c)
                tmp2 := s0 + maj

                h = g
                g = f
                f = e
                e = d + tmp1
                d = c
                c = b
                b = a
                a = tmp1 + tmp2
            }

            result[0] += a
            result[1] += b
            result[2] += c
            result[3] += d
            result[4] += e
            result[5] += f
            result[6] += g
            result[7] += h
        }

        return result
    }
}