/*
    :LICENCE
    This code is based on the Go library (https://github.com/golang/go/blob/master/src/strconv/decimal.go)
    The original code has been modified for Swag
    See LICENCE.md for the corresponding licence.
*/

#global namespace StrConv
#global #[Swag.CanOverflow(true)]
using Globalization, Errors

struct Decimal
{
    d:            [800] u8     // digits, big-endian representation
    nd:           s64          // number of digits used
    dp:           s64          // decimal point
    neg:          bool         // negative flag
    bigtrunc:     bool
    mantissa:     u64
    trunc:        bool
    exp:          s64
}

struct DecimalSlice
{
    d:          [..] u8
    nd, dp:     s64
    neg:        bool
}

struct LeftCheat
{
    delta:      s64        // number of new digits
    cutoff:     string     // minus one digit if original < a.
}

var LeftCheats: [?] LeftCheat = [
    {0, ""},
    {1, "5"}, // * 2
    {1, "25"}, // * 4
    {1, "125"}, // * 8
    {2, "625"}, // * 16
    {2, "3125"}, // * 32
    {2, "15625"}, // * 64
    {3, "78125"}, // * 128
    {3, "390625"}, // * 256
    {3, "1953125"}, // * 512
    {4, "9765625"}, // * 1024
    {4, "48828125"}, // * 2048
    {4, "244140625"}, // * 4096
    {4, "1220703125"}, // * 8192
    {5, "6103515625"}, // * 16384
    {5, "30517578125"}, // * 32768
    {5, "152587890625"}, // * 65536
    {6, "762939453125"}, // * 131072
    {6, "3814697265625"}, // * 262144
    {6, "19073486328125"}, // * 524288
    {7, "95367431640625"}, // * 1048576
    {7, "476837158203125"}, // * 2097152
    {7, "2384185791015625"}, // * 4194304
    {7, "11920928955078125"}, // * 8388608
    {8, "59604644775390625"}, // * 16777216
    {8, "298023223876953125"}, // * 33554432
    {8, "1490116119384765625"}, // * 67108864
    {9, "7450580596923828125"}, // * 134217728
    {9, "37252902984619140625"}, // * 268435456
    {9, "186264514923095703125"}, // * 536870912
    {10, "931322574615478515625"}, // * 1073741824
    {10, "4656612873077392578125"}, // * 2147483648
    {10, "23283064365386962890625"}, // * 4294967296
    {10, "116415321826934814453125"}, // * 8589934592
    {11, "582076609134674072265625"}, // * 17179869184
    {11, "2910383045673370361328125"}, // * 34359738368
    {11, "14551915228366851806640625"}, // * 68719476736
    {12, "72759576141834259033203125"}, // * 137438953472
    {12, "363797880709171295166015625"}, // * 274877906944
    {12, "1818989403545856475830078125"}, // * 549755813888
    {13, "9094947017729282379150390625"}, // * 1099511627776
    {13, "45474735088646411895751953125"}, // * 2199023255552
    {13, "227373675443232059478759765625"}, // * 4398046511104
    {13, "1136868377216160297393798828125"}, // * 8796093022208
    {14, "5684341886080801486968994140625"}, // * 17592186044416
    {14, "28421709430404007434844970703125"}, // * 35184372088832
    {14, "142108547152020037174224853515625"}, // * 70368744177664
    {15, "710542735760100185871124267578125"}, // * 140737488355328
    {15, "3552713678800500929355621337890625"}, // * 281474976710656
    {15, "17763568394002504646778106689453125"}, // * 562949953421312
    {16, "88817841970012523233890533447265625"}, // * 1125899906842624
    {16, "444089209850062616169452667236328125"}, // * 2251799813685248
    {16, "2220446049250313080847263336181640625"}, // * 4503599627370496
    {16, "11102230246251565404236316680908203125"}, // * 9007199254740992
    {17, "55511151231257827021181583404541015625"}, // * 18014398509481984
    {17, "277555756156289135105907917022705078125"}, // * 36028797018963968
    {17, "1387778780781445675529539585113525390625"}, // * 72057594037927936
    {18, "6938893903907228377647697925567626953125"}, // * 144115188075855872
    {18, "34694469519536141888238489627838134765625"}, // * 288230376151711744
    {18, "173472347597680709441192448139190673828125"}, // * 576460752303423488
    {19, "867361737988403547205962240695953369140625"}]

impl Decimal
{
    // If we chop a at nd digits, should we round up?
    mtd shouldRoundUp(nd1: s64)->bool
    {
        if nd1 < 0 or nd1 >= .nd do
            return false
        if .d[nd1] == '5' and nd1 + 1 == .nd // exactly halfway - round to even
        {
            // if we truncated, a little higher than what's recorded - always round up
            if .trunc do
                return true
            return nd1 > 0 and (.d[nd1 - 1] - '0') % 2 != 0
        }

        // not halfway - digit tells all
        return .d[nd1] >= '5'
    }

    // Round a to nd digits (or fewer).
    // If nd is zero, it means we're rounding
    // just to the left of the digits, as in
    // 0.09 -> 0.1.
    mtd round(nd1: s64)
    {
        if nd1 < 0 or nd1 >= .nd do
            return
        if .shouldRoundUp(nd1) do
            .roundUp(nd1)
        else do
            .roundDown(nd1)
    }

    // Round a down to nd digits (or fewer).
    mtd roundDown(nd1: s64)
    {
        if nd1 < 0 or nd1 >= .nd do
            return
        .nd = nd1
        .trim()
    }

    // Round a up to nd digits (or fewer).
    mtd roundUp(nd1: s64)
    {
        if nd1 < 0 or nd1 >= .nd do
            return
        // round up
        for var i = nd1 - 1; i >= 0; i -= 1
        {
            let c = .d[i]
            if c < '9' // can stop after this digit
            {
                .d[i] += 1
                .nd = i + 1
                return
            }
        }

        // Number is all 9s.
        // Change to single 1 with adjusted decimal point.
        .d[0] = '1'
        .nd   = 1
        .dp += 1
    }

    mtd assign(mant: u64)
    {
        var buf: [24] u8

        // Write reversed decimal in buf.
        var v = mant
        var n = 0
        while v > 0
        {
            let v1 = v / 10
            v -= 10 * v1
            buf[n] = cast(u8) (v + '0')
            n += 1
            v = v1
        }

        // Reverse again to produce forward decimal in a.d.
        .nd = 0
        n -= 1
        while n >= 0
        {
            .d[.nd] = buf[n]
            .nd += 1
            n -= 1
        }

        .dp = .nd
        .trim()
    }

    mtd trim()
    {
        while .nd > 0 and .d[.nd - 1] == '0' do
            .nd -= 1
        if .nd == 0 do
            .dp = 0
    }

    const MaxShift: u32 = 64 - 4
    mtd rightShift(k: u32)
    {
        var r = 0     // read pointer
        var w = 0     // write pointer

        // Pick up enough leading digits to cover first shift.
        var n: u64
        while n >> k == 0
        {
            if r >= .nd
            {
                if n == 0
                {
                    .nd = 0
                    return
                }

                while n >> k == 0
                {
                    n = n * 10
                    r += 1
                }

                break
            }

            let c = cast(u64) .d[r]
            n = (n * 10) + c - '0'
            r += 1
        }

        .dp -= r - 1

        let mask: u64 = (1'u64 << k) - 1

        // Pick up a digit, put down a digit.
        while r < .nd
        {
            let c   = cast(u64) .d[r]
            let dig = n >> k
            n &= mask
            .d[w] = cast(u8) (dig + '0')
            w += 1
            n = (n * 10) + c - '0'
            r += 1
        }

        // Put down extra digits.
        while n > 0
        {
            let dig = n >> k
            n &= mask
            if w < @countof(.d)
            {
                .d[w] = cast(u8) (dig + '0')
                w += 1
            }
            elif dig > 0
            {
                .trunc = true
            }

            n *= 10
        }

        .nd = w
        .trim()
    }

    func prefixIsLessThan(b: const [..] u8, s: string)->bool
    {
        for var i = 0; i < @countof(s); i += 1
        {
            if i >= @countof(b) do
                return true
            if b[i] != s[i] do
                return b[i] < s[i]
        }

        return false
    }

    mtd leftShift(k: u32)
    {
        var delta = LeftCheats[k].delta
        if prefixIsLessThan(@mkslice(&.d[0], .nd), LeftCheats[k].cutoff) do
            delta -= 1

        var r = .nd             // read index
        var w = .nd + delta     // write index

        // Pick up a digit, put down a digit.
        var n: u64
        r -= 1
        while r >= 0
        {
            n += (cast(u64) .d[r] - '0') << k
            let quo = n / 10
            let rem = n - (10 * quo)
            w -= 1
            if w < @countof(.d) do
                .d[w] = cast(u8) (rem + '0')
            elif rem != 0 do
                .trunc = true
            n = quo
            r -= 1
        }

        // Put down extra digits.
        while n > 0
        {
            let quo = n / 10
            let rem = n - (10 * quo)
            w -= 1
            if w < @countof(.d) do
                .d[w] = cast(u8) (rem + '0')
            elif rem != 0 do
                .trunc = true
            n = quo
        }

        .nd += delta
        if .nd >= @countof(.d) do
            .nd = cast(s32) @countof(.d)
        .dp += delta
        .trim()
    }

    mtd shift(kk: s32)
    {
        var k = kk
        switch
        {
        case .nd == 0:
            break

        case k > 0:
            while k > MaxShift
            {
                .leftShift(MaxShift)
                k -= MaxShift
            }

            .leftShift(cast(u32) k)

        case k < 0:
            while k < -cast(s32) MaxShift
            {
                .rightShift(MaxShift)
                k += MaxShift
            }

            .rightShift(cast(u32) -k)
        }
    }

    mtd readF64(str: const [..] u8)->u32 throw
    {
        var cpt     = 0'u32
        let countof = @countof(str)
        var buffer  = @dataof(str)

        let negSign  = g_CultureInfo.numberFormat.negativeSign
        let plusSign = g_CultureInfo.numberFormat.positiveSign
        let dotSign  = g_CultureInfo.numberFormat.decimalSeparator

        var c = dref buffer
        if c == negSign
        {
            buffer, cpt += 1
            .neg = true
            if !Latin1.isDigit(dref buffer) and cpt == countof do
                throw ParseIncompleteError{}
        }
        elif c == plusSign
        {
            buffer, cpt += 1
            if !Latin1.isDigit(dref buffer) and cpt == countof do
                throw ParseIncompleteError{}
        }

        // digits
        let maxMantDigits = 19        // 10^19 fits in u64
        var sawdot        = false
        var sawDigits     = false
        var ndMant        = 0

        .nd = 0
        .dp = 0

        while cpt < countof
        {
            c = dref buffer
            if c == dotSign
            {
                if sawdot do
                    break
                sawdot = true
                .dp    = .nd
            }
            elif '0' <= c and c <= '9'
            {
                sawDigits = true
                if c == '0' and .nd == 0 do
                    .dp -= 1
                else
                {
                    if .nd < @countof(.d)
                    {
                        .d[.nd] = c
                        .nd += 1

                        if ndMant < maxMantDigits
                        {
                            .mantissa *= 10
                            .mantissa += cast(u64) (c - '0')
                            ndMant += 1
                        }
                        elif c != '0' do
                            .trunc = true
                    }
                    elif c != '0' do
                        .bigtrunc = true
                }
            }
            else do
                break

            cpt, buffer += 1
        }

        if !sawDigits do
            throw SyntaxError{}
        if !sawdot do
            .dp = .nd

        // Exponent
        c = dref buffer
        if cpt < countof and (c | 32) == 'e'
        {
            cpt, buffer += 1
            if cpt >= countof do
                throw ParseIncompleteError{}

            var esign = 1
            c = dref buffer
            if c == negSign
            {
                buffer, cpt += 1
                esign = -1
                c     = dref buffer
                if !Latin1.isDigit(c) and cpt >= countof do
                    throw ParseIncompleteError{}
            }
            elif c == plusSign
            {
                buffer, cpt += 1
                c = dref buffer
                if !Latin1.isDigit(c) and cpt >= countof do
                    throw ParseIncompleteError{}
            }

            if cpt >= countof or c < '0' or c > '9' do
                throw SyntaxError{}

            var e = 0
            while cpt < countof
            {
                c = dref buffer
                if c < '0' or c > '9' do
                    break
                if e < 10000 do
                    e = e * 10 + (c - '0')
                buffer, cpt += 1
            }

            .dp += e * esign
        }

        if .mantissa != 0 do
            .exp = .dp - ndMant

        return cpt
    }

    mtd floatBits()->{ b: u64, overflow: bool }
    {
        const PowTab: [?] s64 = [1, 3, 6, 9, 13, 16, 19, 23, 26]
        var mant:     u64

        // Zero is always a special case.
        var overflow = false
        for
        {
            if .nd == 0
            {
                mant = 0
                .exp = Swag.F64.Bias
                break
            }

            // Obvious overflow/underflow.
            // These bounds are for 64-bit floats.
            // Will have to change if we want to support 80-bit floats in the future.
            if .dp > 310
            {
                overflow = true
                break
            }

            if .dp < -330
            {
                // zero
                mant = 0
                .exp = Swag.F64.Bias
                break
            }

            // Scale by powers of two until in range [0.5, 1.0)
            .exp = 0
            while .dp > 0
            {
                var n: s64
                if .dp >= @countof(PowTab) do
                    n = 27
                else do
                    n = PowTab[.dp]
                .shift(-cast(s32) n)
                .exp += n
            }

            while .dp < 0 or .dp == 0 and .d[0] < '5'
            {
                var n: s64
                if -.dp >= @countof(PowTab) do
                    n = 27
                else do
                    n = PowTab[-.dp]
                .shift(cast(s32) n)
                .exp -= n
            }

            // Our range is [0.5,1) but floating point range is [1,2).
            .exp -= 1

            // Minimum representable exponent is flt.bias+1.
            // If the exponent is smaller, #move it up and
            // adjust d accordingly.
            if .exp < Swag.F64.Bias + 1
            {
                let n = Swag.F64.Bias + 1 - .exp
                .shift(-cast(s32) n)
                .exp += n
            }

            if .exp - Swag.F64.Bias >= (1'u64 << Swag.F64.ExpBits) - 1
            {
                overflow = true
                break
            }

            // Extract 1 + Swag.F64.MantBits bits.
            .shift(cast(s32) (1 + Swag.F64.MantBits))
            mant = .roundedInteger()

            // Rounding might have added a bit; shift down.
            if mant == 2'u64 << Swag.F64.MantBits
            {
                mant >>= 1
                .exp += 1
                if .exp - Swag.F64.Bias >= (1 << Swag.F64.ExpBits) - 1
                {
                    overflow = true
                    break
                }
            }

            // Denormalized?
            if mant & (1'u64 << Swag.F64.MantBits) == 0 do
                .exp = Swag.F64.Bias
            break
        }

        if overflow
        {
            mant = 0
            .exp = cast(s64) ((1'u64 << Swag.F64.MantBits) - 1 + cast #wrap (u64) Swag.F64.Bias)
        }

        // Assemble bits.
        var result: retval
        var bits    = mant & ((1'u64 << Swag.F64.MantBits) - 1)
        bits |= cast(u64) ((.exp - Swag.F64.Bias) & ((1's64 << Swag.F64.MantBits) - 1)) << Swag.F64.MantBits
        if .neg do
            bits |= (1'u64 << 63)
        result.b        = bits
        result.overflow = overflow
        return result
    }

    mtd roundedInteger()->u64
    {
        if .dp > 20 do
            return 0xFFFFFFFFFFFFFFFF

        var i = 0's64
        var n = 0'u64
        while i < .dp and i < .nd
        {
            n = (n * 10) + cast(u64) (.d[i] - '0')
            i += 1
        }

        while i < .dp
        {
            n *= 10
            i += 1
        }

        if .shouldRoundUp(.dp) do
            n += 1
        return n
    }
}
