/*
    :LICENCE
    Mt19937-64 Mersenne Twister 64 bits.
    This code is based on the C-program by Matt Gallagher
    (https://github.com/mattgallagher/CwlUtils/blob/master/Sources/ReferenceRandomGenerators/mt19937-64.c)
    (http://www.cocoawithlove.com/blog/2016/05/19/random-numbers.html)
    Original code by Takuji Nishimura and Makoto Matsumoto.
    See LICENCE.md for the corresponding licence.
*/

#global namespace Random

public const NN = 312

const MID      = NN / 2
const MM       = 156
const MATRIX_A = 0xB5026F5AA96619E9'u64
const UM       = 0xFFFFFFFF80000000'u64
const LM       = 0x7FFFFFFF'u64

public struct Mt64
{
    mt:      [NN] u64
    mti:     u64
}

public impl Mt64
{
    #[Swag.CanOverflow(true), Swag.Overload]
    mtd seedU64(seed: u64)
    {
        .mt[0] = seed
        .mti   = 1
        while .mti < NN
        {
            .mt[.mti] = (6364136223846793005'u64 * (.mt[.mti - 1] ^ (.mt[.mti - 1] >> 62)) + .mti)
            .mti += 1
        }
    }

    #[Swag.CanOverflow(true), Swag.Overload]
    mtd seedU64(seed: const [..] u64)
    {
        let keyLength = @countof(seed)

        .seedU64(19650218)
        var i = 1
        var j = 0
        var k = NN > keyLength ? NN : keyLength

        while k
        {
            .mt[i] = (.mt[i] ^ ((.mt[i - 1] ^ (.mt[i - 1] >> 62)) * 3935559000370003845)) + seed[j] + j
            i, j += 1
            if i >= NN
            {
                .mt[0] = .mt[NN - 1]
                i      = 1
            }
            if j >= keyLength do
                j = 0
            k -= 1
        }

        k = NN - 1
        while k != 0
        {
            .mt[i] = (.mt[i] ^ ((.mt[i - 1] ^ (.mt[i - 1] >> 62)) * 2862933555777941757)) - i
            i += 1
            if i >= NN
            {
                .mt[0] = .mt[NN - 1]
                i      = 1
            }
            k -= 1
        }

        .mt[0] = 1'u64 << 63
    }

    #[Swag.CanOverflow(true)]
    mtd nextU64()->u64
    {
        if .mti >= NN
        {
            let stateMid = .mt[MID]

            for var i = 0, j = MID; i != MID - 1; i, j += 1
            {
                let x = (.mt[i] & UM) | (.mt[i + 1] & LM)
                .mt[i] = .mt[i + MID] ^ (x >> 1) ^ ((.mt[i + 1] & 1) * MATRIX_A)
                let y = (.mt[j] & UM) | (.mt[j + 1] & LM)
                .mt[j] = .mt[j - MID] ^ (y >> 1) ^ ((.mt[j + 1] & 1) * MATRIX_A)
            }

            let x = (.mt[MID - 1] & UM) | (stateMid & LM)
            .mt[MID - 1] = .mt[NN - 1] ^ (x >> 1) ^ ((stateMid & 1) * MATRIX_A)
            let y = (.mt[NN - 1] & UM) | (.mt[0] & LM)
            .mt[NN - 1] = .mt[MID - 1] ^ (y >> 1) ^ ((.mt[0] & 1) * MATRIX_A)

            .mti = 0
        }

        var result = .mt[.mti]
        .mti += 1

        result ^= (result >> 29) & 0x5555555555555555'u64
        result ^= (result << 17) & 0x71D67FFFEDA60000'u64
        result ^= (result << 37) & 0xFFF7EEE000000000'u64
        result ^= (result >> 43)
        return result
    }
}
