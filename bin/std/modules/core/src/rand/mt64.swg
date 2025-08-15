/*
    :LICENCE
    Mt19937-64 Mersenne Twister 64 bits.
    This code is based on the C-program by Matt Gallagher
    (https://github.com/mattgallagher/CwlUtils/blob/master/Sources/ReferenceRandomGenerators/mt19937-64.c)
    (http://www.cocoawithlove.com/blog/2016/05/19/random-numbers.html)
    Original code by Takuji Nishimura and Makoto Matsumoto.
    See LICENCE.md for the corresponding licence.
*/

#global marked
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
        self.mt[0] = seed
        self.mti   = 1
        while self.mti < NN
        {
            self.mt[self.mti] = (6364136223846793005'u64 * (self.mt[self.mti - 1] ^ (self.mt[self.mti - 1] >> 62)) + self.mti)
            self.mti += 1
        }
    }

    #[Swag.CanOverflow(true), Swag.Overload]
    mtd seedU64(seed: const [..] u64)
    {
        let keyLength = @countof(seed)

        self.seedU64(19650218)
        var i = 1
        var j = 0
        var k = NN > keyLength ? NN : keyLength

        while k
        {
            self.mt[i] = (self.mt[i] ^ ((self.mt[i - 1] ^ (self.mt[i - 1] >> 62)) * 3935559000370003845)) + seed[j] + j
            i, j += 1
            if i >= NN
            {
                self.mt[0] = self.mt[NN - 1]
                i     = 1
            }
            if j >= keyLength:
                j = 0
            k -= 1
        }

        k = NN - 1
        while k != 0
        {
            self.mt[i] = (self.mt[i] ^ ((self.mt[i - 1] ^ (self.mt[i - 1] >> 62)) * 2862933555777941757)) - i
            i += 1
            if i >= NN
            {
                self.mt[0] = self.mt[NN - 1]
                i     = 1
            }
            k -= 1
        }

        self.mt[0] = 1'u64 << 63
    }

    #[Swag.CanOverflow(true)]
    mtd nextU64()->u64
    {
        if self.mti >= NN
        {
            let stateMid = self.mt[MID]

            for var i = 0, j = MID; i != MID - 1; i, j += 1
            {
                let x = (self.mt[i] & UM) | (self.mt[i + 1] & LM)
                self.mt[i] = self.mt[i + MID] ^ (x >> 1) ^ ((self.mt[i + 1] & 1) * MATRIX_A)
                let y = (self.mt[j] & UM) | (self.mt[j + 1] & LM)
                self.mt[j] = self.mt[j - MID] ^ (y >> 1) ^ ((self.mt[j + 1] & 1) * MATRIX_A)
            }

            let x = (self.mt[MID - 1] & UM) | (stateMid & LM)
            self.mt[MID - 1] = self.mt[NN - 1] ^ (x >> 1) ^ ((stateMid & 1) * MATRIX_A)
            let y = (self.mt[NN - 1] & UM) | (self.mt[0] & LM)
            self.mt[NN - 1] = self.mt[MID - 1] ^ (y >> 1) ^ ((self.mt[0] & 1) * MATRIX_A)

            self.mti = 0
        }

        var result = self.mt[self.mti]
        self.mti += 1

        result ^= (result >> 29) & 0x5555555555555555'u64
        result ^= (result << 17) & 0x71D67FFFEDA60000'u64
        result ^= (result << 37) & 0xFFF7EEE000000000'u64
        result ^= (result >> 43)
        return result
    }
}
