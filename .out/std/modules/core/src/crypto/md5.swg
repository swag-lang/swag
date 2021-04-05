#global namespace hash
#[swag.safety("of", false)]
{
#[swag.inline]
private func f1(x, y, z: u32)->u32
{
    return (x & y) | (~x & z)
}

#[swag.inline]
private func f2(x, y, z: u32)->u32
{
    return (x & z) | (y & ~z)
}

#[swag.inline]
private func f3(x, y, z: u32)->u32
{
    return x ^ (y ^ z)
}

#[swag.inline]
private func f4(x, y, z: u32)->u32
{
    return y ^ (x | ~z)
}

#[swag.inline]
private func f5(x, n: u32)->u32
{
    return (x << n) | (x >> (32 - n))
}

#[swag.inline]
private func ff1(a: *u32, b, c, d, x, s, ac: u32)
{
    :a += f1(b, c, d) + x + ac
    :a = f5(:a, s)
    :a += b
}

#[swag.inline]
private func ff2(a: *u32, b, c, d, x, s, ac: u32)
{
    :a += f2(b, c, d) + x + ac
    :a = f5(:a, s)
    :a += b
}

#[swag.inline]
private func ff3(a: *u32, b, c, d, x, s, ac: u32)
{
    :a += f3(b, c, d) + x + ac
    :a = f5(:a, s)
    :a += b
}

#[swag.inline]
private func ff4(a: *u32, b, c, d, x, s, ac: u32)
{
    :a += f4(b, c, d) + x + ac
    :a = f5(:a, s)
    :a += b
}

private func transform(state: [4] u32, block: const *u8)
{
    a := state[0]
    b := state[1]
    c := state[2]
    d := state[3]
    var x: [16] u32

    decode(x, block, 64)

    // Round 1
    ff1(&a, b, c, d, x[0], 7, 0xd76aa478)   // 1
    ff1(&d, a, b, c, x[1], 12, 0xe8c7b756)  // 2
    ff1(&c, d, a, b, x[2], 17, 0x242070db)  // 3
    ff1(&b, c, d, a, x[3], 22, 0xc1bdceee)  // 4
    ff1(&a, b, c, d, x[4], 7, 0xf57c0faf)   // 5
    ff1(&d, a, b, c, x[5], 12, 0x4787c62a)  // 6
    ff1(&c, d, a, b, x[6], 17, 0xa8304613)  // 7
    ff1(&b, c, d, a, x[7], 22, 0xfd469501)  // 8
    ff1(&a, b, c, d, x[8], 7, 0x698098d8)   // 9
    ff1(&d, a, b, c, x[9], 12, 0x8b44f7af)  // 10
    ff1(&c, d, a, b, x[10], 17, 0xffff5bb1) // 11
    ff1(&b, c, d, a, x[11], 22, 0x895cd7be) // 12
    ff1(&a, b, c, d, x[12], 7, 0x6b901122)  // 13
    ff1(&d, a, b, c, x[13], 12, 0xfd987193) // 14
    ff1(&c, d, a, b, x[14], 17, 0xa679438e) // 15
    ff1(&b, c, d, a, x[15], 22, 0x49b40821) // 16

    // Round 2
    ff2(&a, b, c, d, x[1], 5, 0xf61e2562)   // 17
    ff2(&d, a, b, c, x[6], 9, 0xc040b340)   // 18
    ff2(&c, d, a, b, x[11], 14, 0x265e5a51) // 19
    ff2(&b, c, d, a, x[0], 20, 0xe9b6c7aa)  // 20
    ff2(&a, b, c, d, x[5], 5, 0xd62f105d)   // 21
    ff2(&d, a, b, c, x[10], 9, 0x2441453)   // 22
    ff2(&c, d, a, b, x[15], 14, 0xd8a1e681) // 23
    ff2(&b, c, d, a, x[4], 20, 0xe7d3fbc8)  // 24
    ff2(&a, b, c, d, x[9], 5, 0x21e1cde6)   // 25
    ff2(&d, a, b, c, x[14], 9, 0xc33707d6)  // 26
    ff2(&c, d, a, b, x[3], 14, 0xf4d50d87)  // 27

    ff2(&b, c, d, a, x[8], 20, 0x455a14ed)  // 28
    ff2(&a, b, c, d, x[13], 5, 0xa9e3e905)  // 29
    ff2(&d, a, b, c, x[2], 9, 0xfcefa3f8)   // 30
    ff2(&c, d, a, b, x[7], 14, 0x676f02d9)  // 31
    ff2(&b, c, d, a, x[12], 20, 0x8d2a4c8a) // 32

    // Round 3
    ff3(&a, b, c, d, x[5], 4, 0xfffa3942)   // 33
    ff3(&d, a, b, c, x[8], 11, 0x8771f681)  // 34
    ff3(&c, d, a, b, x[11], 16, 0x6d9d6122) // 35
    ff3(&b, c, d, a, x[14], 23, 0xfde5380c) // 36
    ff3(&a, b, c, d, x[1], 4, 0xa4beea44)   // 37
    ff3(&d, a, b, c, x[4], 11, 0x4bdecfa9)  // 38
    ff3(&c, d, a, b, x[7], 16, 0xf6bb4b60)  // 39
    ff3(&b, c, d, a, x[10], 23, 0xbebfbc70) // 40
    ff3(&a, b, c, d, x[13], 4, 0x289b7ec6)  // 41
    ff3(&d, a, b, c, x[0], 11, 0xeaa127fa)  // 42
    ff3(&c, d, a, b, x[3], 16, 0xd4ef3085)  // 43
    ff3(&b, c, d, a, x[6], 23, 0x4881d05)   // 44
    ff3(&a, b, c, d, x[9], 4, 0xd9d4d039)   // 45
    ff3(&d, a, b, c, x[12], 11, 0xe6db99e5) // 46
    ff3(&c, d, a, b, x[15], 16, 0x1fa27cf8) // 47
    ff3(&b, c, d, a, x[2], 23, 0xc4ac5665)  // 48

    // Round 4
    ff4(&a, b, c, d, x[0], 6, 0xf4292244)   // 49
    ff4(&d, a, b, c, x[7], 10, 0x432aff97)  // 50
    ff4(&c, d, a, b, x[14], 15, 0xab9423a7) // 51
    ff4(&b, c, d, a, x[5], 21, 0xfc93a039)  // 52
    ff4(&a, b, c, d, x[12], 6, 0x655b59c3)  // 53
    ff4(&d, a, b, c, x[3], 10, 0x8f0ccc92)  // 54
    ff4(&c, d, a, b, x[10], 15, 0xffeff47d) // 55
    ff4(&b, c, d, a, x[1], 21, 0x85845dd1)  // 56
    ff4(&a, b, c, d, x[8], 6, 0x6fa87e4f)   // 57
    ff4(&d, a, b, c, x[15], 10, 0xfe2ce6e0) // 58
    ff4(&c, d, a, b, x[6], 15, 0xa3014314)  // 59
    ff4(&b, c, d, a, x[13], 21, 0x4e0811a1) // 60
    ff4(&a, b, c, d, x[4], 6, 0xf7537e82)   // 61
    ff4(&d, a, b, c, x[11], 10, 0xbd3af235) // 62
    ff4(&c, d, a, b, x[2], 15, 0x2ad7d2bb)  // 63
    ff4(&b, c, d, a, x[9], 21, 0xeb86d391)  // 64

    state[0] += a
    state[1] += b
    state[2] += c
    state[3] += d
}

func encode(output: *u8, input: *u32, len: u32)
{
    i, j := 0
    while j < len
    {
        output[j]     = cast(u8) (input[i] & 0xff)
        output[j + 1] = cast(u8) ((input[i] >> 8) & 0xff)
        output[j + 2] = cast(u8) ((input[i] >> 16) & 0xff)
        output[j + 3] = cast(u8) ((input[i] >> 24) & 0xff)
        i += 1
        j += 4
    }
}

private func decode(output: *u32, input: const *u8, len: u32)
{
    var i, j: u32
    while j < len
    {
        output[i] = (cast(u32) input[j]) |
                    ((cast(u32) input[j + 1]) << 8) |
                    ((cast(u32) input[j + 2]) << 16) |
                    ((cast(u32) input[j + 3]) << 24)
        i += 1
        j += 4
    }
}

public struct Md5
{
    padding:    [64] u8
    state:      [4] u32
    count:      [2] u32
    buffer:     [64] u8
}

public impl Md5
{
    // Call to finalize the computation, when your done
    func final(using self, digest: [16] u8)
    {
        var bits: [8] u8
        encode(&bits[0], count, 8)

        index  := (count[0] >> 3) & 0x3f
        padLen := (index < 56) ? (56 - index) : (120 - index)

        update(@mkslice(&padding[0], padLen))
        update(@mkslice(&bits[0], 8))
        encode(&digest[0], state, 16)
    }

    // Initialize the hashing sequence
    func init(using self)
    {
        @init(self)
        padding[0] = 0x80
        state[0] = 0x67452301
        state[1] = 0xefcdab89
        state[2] = 0x98badcfe
        state[3] = 0x10325476
    }

    // Update the hash value with 'buffer' content
    func update(using self, data: const [..] u8)
    {
        input := @dataof(data)
        inputLen := cast(u32) @countof(data)

        index := (count[0] >> 3) & 0x3F
        count[0] += (inputLen << 3)
        if count[0] < (inputLen << 3)
            count[1] += 1
        count[1] += inputLen >>% 29

        var i: u32

        partLen := 64 - index
        if inputLen >= partLen
        {
            memory.copy(&buffer[index], input, cast(uint) partLen)
            transform(state, &buffer[0])
            i = partLen
            while i + 63 < inputLen
            {
                transform(state, input + i)
                i += 64
            }

            index = 0
        }

        memory.copy(&buffer[index], &input[i], cast(uint) inputLen - i)
    }

    // Returns the md5 value for the given buffer
    func compute(digest: [16] u8, buffer: const [..] u8)
    {
        var v: Md5
        v.init()
        v.update(buffer)
        v.final(digest)
    }
}
}