public namespace hash;

// https://tools.ietf.org/html/rfc1950#section-9
// https://github.com/madler/zlib/blob/master/adler32.c

private
{
    const Base = 65521
    const NMax = 5552
}

struct Adler32
{
    adler: u32
}

impl Adler32
{
    // Initialize the hashing sequence
    func init(using self)
    {
        adler = 1
    }

    // Update the hash value with 'buffer' content
    func update(using self, buffer: const [..] u8)
    {
        s1 := adler & 0xffff
        s2 := (adler >> 16) & 0xffff

        countof := @countof(buffer)
        if !countof return

        input := @dataof(buffer)
        if countof == 1
        {
            s1 += input[0]
            if s1 >= Base
                s1 -= Base
            s2 += s1
            if s2 >= Base
                s2 -= Base
        }
        else if countof < 16
        {
            visit b: buffer
            {
                s1 += b
                s2 += s1
            }

            if s1 >= Base
                s1 -= Base
            s2 %= Base
        }
        else
        {
            const n = NMax / 16
            i := 0'uint

            while i + NMax <= countof
            {
                rounds := 0'uint
                for ; rounds < n; rounds += 1
                {
                    j := 0'uint
                    for ; j < 16; j += 1
                    {
                        s1 += input[i + j]
                        s2 += s1
                    }

                    i += 16
                }

                s1 %= Base
                s2 %= Base
            }

            if (i < countof)
            {
                for ; i + 16 <= countof; i += 16
                {
                    j := 0'uint
                    for ; j < 16; j += 1
                    {
                        s1 += input[i + j]
                        s2 += s1
                    }
                }

                for ; i < countof; i += 1
                {
                    s1 += input[i]
                    s2 += s1
                }

                s1 %= Base
                s2 %= Base
            }
        }

        adler = s1 | (s2 << 16)
    }

    // Returns the adler32 hash value for the given buffer
    func hash(buffer: const [..] u8)->u32
    {
        var v: Adler32
        v.init()
        v.update(buffer)
        return v.adler
    }
}