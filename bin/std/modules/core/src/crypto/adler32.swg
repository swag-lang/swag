// https://tools.ietf.org/html/rfc1950#section-9
// https://github.com/madler/zlib/blob/master/adler32.c

#global namespace Hash

public struct Adler32
{
    adler: u32 = 1
}

public impl Adler32
{
    internal const Base = 65521
    internal const NMax = 5552

    // Initialize the hashing sequence
    mtd init()
    {
        .adler = 1
    }

    // Update the hash value with 'buffer' content
    mtd update(buffer: const [..] u8)
    {
        var s1 = .adler & 0xffff
        var s2 = (.adler >> 16) & 0xffff

        let countof = @countof(buffer)
        if !countof do
            return

        let input = @dataof(buffer)
        if countof == 1
        {
            s1 += input[0]
            if s1 >= Base do
                s1 -= Base
            s2 += s1
            if s2 >= Base do
                s2 -= Base
        }
        elif countof < 16
        {
            foreach b in buffer
            {
                s1 += b
                s2 += s1
            }

            if s1 >= Base do
                s1 -= Base
            s2 %= Base
        }
        else
        {
            const n = NMax / 16
            var i   = 0'u64

            while i + NMax <= countof
            {
                for rounds in n
                {
                    for j in 16
                    {
                        s1 += input[i + j]
                        s2 += s1
                    }

                    i += 16
                }

                s1 %= Base
                s2 %= Base
            }

            if i < countof
            {
                while i + 16 <= countof
                {
                    for j in 16
                    {
                        s1 += input[i + j]
                        s2 += s1
                    }

                    i += 16
                }

                while i < countof
                {
                    s1 += input[i]
                    s2 += s1
                    i += 1
                }

                s1 %= Base
                s2 %= Base
            }
        }

        .adler = s1 | (s2 << 16)
    }

    // Returns the adler32 hash value for the given buffer
    #[Swag.ConstExpr]
    func compute(buffer: const [..] u8)->u32
    {
        var v: Adler32
        v.init()
        v.update(buffer)
        return v.adler
    }
}
