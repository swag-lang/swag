#global namespace hash
#global #[swag.safety("of", false)]

func ROTLEFT(a,b: u32)  => (((a) << (b)) | ((a) >> (32-(b))))
func ROTRIGHT(a,b: u32) => (((a) >> (b)) | ((a) << (32-(b))))
func CH(x,y,z: u32)     => (((x) & (y)) ^ (~(x) & (z)))
func MAJ(x,y,z: u32)    => (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
func EP0(x: u32)        => (ROTRIGHT(x,2) ^ ROTRIGHT(x,13) ^ ROTRIGHT(x,22))
func EP1(x: u32)        => (ROTRIGHT(x,6) ^ ROTRIGHT(x,11) ^ ROTRIGHT(x,25))
func SIG0(x: u32)       => (ROTRIGHT(x,7) ^ ROTRIGHT(x,18) ^ ((x) >> 3))
func SIG1(x: u32)       => (ROTRIGHT(x,17) ^ ROTRIGHT(x,19) ^ ((x) >> 10))

const k: [64] u32 = @[
	0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
	0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
	0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
	0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
	0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
	0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
	0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
	0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
]

public struct Sha256
{
    data:       [64] u8
    state:      [8] u32
    datalen:    u32
    bitlen:     u64
}

public impl Sha256
{
    private func transform(using self, buf: const *u8)
    {
        var m: [64] u32

        i, j := 0'u32
        while i < 16
        {
            m[i] = (buf[j] << 24) | (buf[j + 1] << 16) | (buf[j + 2] << 8) | (buf[j + 3])
            i += 1
            j += 4
        }

        while i < 64
        {
            m[i] = SIG1(m[i - 2]) + m[i - 7] + SIG0(m[i - 15]) + m[i - 16]
            i += 1
        }

        a := state[0]
        b := state[1]
        c := state[2]
        d := state[3]
        e := state[4]
        f := state[5]
        g := state[6]
        h := state[7]

        i = 0
        while i < 64
        {
            t1 := h + EP1(e) + CH(e,f,g) + k[i] + m[i]
            t2 := EP0(a) + MAJ(a,b,c)
            h = g
            g = f
            f = e
            e = d + t1
            d = c
            c = b
            b = a
            a = t1 + t2
            i += 1
        }

        state[0] += a
        state[1] += b
        state[2] += c
        state[3] += d
        state[4] += e
        state[5] += f
        state[6] += g
        state[7] += h
    }

    func init(using self)
    {
        @init(self)
        state[0] = 0x6a09e667
        state[1] = 0xbb67ae85
        state[2] = 0x3c6ef372
        state[3] = 0xa54ff53a
        state[4] = 0x510e527f
        state[5] = 0x9b05688c
        state[6] = 0x1f83d9ab
        state[7] = 0x5be0cd19
    }

    func final(using self, hash: *u32)
    {
        i := datalen

        // Pad whatever data is left in the buffer.
        if (datalen < 56)
        {
            data[i] = 0x80
            i += 1
            while i < 56
            {
                data[i] = 0x00
                i += 1
            }
        }
        else
        {
            data[i] = 0x80
            i += 1
            while i < 64
            {
                data[i] = 0x00
                i += 1
            }

            transform(data)
        }

        // Append to the padding the total message's length in bits and transform.
        bitlen += datalen * 8
        data[63] = bitcast(u8) (bitlen)
        data[62] = bitcast(u8) (bitlen >> 8)
        data[61] = bitcast(u8) (bitlen >> 16)
        data[60] = bitcast(u8) (bitlen >> 24)
        data[59] = bitcast(u8) (bitlen >> 32)
        data[58] = bitcast(u8) (bitlen >> 40)
        data[57] = bitcast(u8) (bitlen >> 48)
        data[56] = bitcast(u8) (bitlen >> 56)
        transform(data)

        // Copy the result
        memory.copy(hash, &state[0], @sizeof(state))
    }

    func update(using self, buffer: const [..] u8)
    {
        debug.assert(@countof(buffer) <= swag.MaxU32)
        buf := @dataof(buffer)
        len := cast(u32) @countof(buffer)

        loop i: len
        {
            data[datalen] = buf[i]
            datalen += 1
            if datalen == 64
            {
                transform(data)
                bitlen += 512
                datalen = 0
            }
        }
    }

    // Returns the sha256 value for the given buffer
    func compute(buffer: const [..] u8)->[8] u32
    {
        var v: Sha256
        v.init()
        v.update(buffer)

        var result: retval
        v.final(&result[0])
        return result
    }
}