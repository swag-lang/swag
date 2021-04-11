// https://github.com/golang/go/blob/master/src/strconv/decimal.go

#global namespace strconv
#global #[swag.safety("of", false)]
using globalization

struct Decimal
{
	d:          [800] u8  // digits, big-endian representation
	nd:         s64       // number of digits used
	dp:         s64       // decimal point
	neg:        bool      // negative flag
	bigtrunc:   bool      // discarded nonzero digits beyond d[:nd]
    mantissa:   u64
	trunc:      bool      // discarded nonzero digits beyond d[:nd]
    exp:        s64
}

struct DecimalSlice
{
	d:      [..] u8
	nd, dp: s64
	neg:    bool
}

struct LeftCheat
{
	delta:  s64    // number of new digits
	cutoff: string // minus one digit if original < a.
}

var LeftCheats: [] LeftCheat = @[
	{0, ""},
	{1, "5"},                                           // * 2
	{1, "25"},                                          // * 4
	{1, "125"},                                         // * 8
	{2, "625"},                                         // * 16
	{2, "3125"},                                        // * 32
	{2, "15625"},                                       // * 64
	{3, "78125"},                                       // * 128
	{3, "390625"},                                      // * 256
	{3, "1953125"},                                     // * 512
	{4, "9765625"},                                     // * 1024
	{4, "48828125"},                                    // * 2048
	{4, "244140625"},                                   // * 4096
	{4, "1220703125"},                                  // * 8192
	{5, "6103515625"},                                  // * 16384
	{5, "30517578125"},                                 // * 32768
	{5, "152587890625"},                                // * 65536
	{6, "762939453125"},                                // * 131072
	{6, "3814697265625"},                               // * 262144
	{6, "19073486328125"},                              // * 524288
	{7, "95367431640625"},                              // * 1048576
	{7, "476837158203125"},                             // * 2097152
	{7, "2384185791015625"},                            // * 4194304
	{7, "11920928955078125"},                           // * 8388608
	{8, "59604644775390625"},                           // * 16777216
	{8, "298023223876953125"},                          // * 33554432
	{8, "1490116119384765625"},                         // * 67108864
	{9, "7450580596923828125"},                         // * 134217728
	{9, "37252902984619140625"},                        // * 268435456
	{9, "186264514923095703125"},                       // * 536870912
	{10, "931322574615478515625"},                      // * 1073741824
	{10, "4656612873077392578125"},                     // * 2147483648
	{10, "23283064365386962890625"},                    // * 4294967296
	{10, "116415321826934814453125"},                   // * 8589934592
	{11, "582076609134674072265625"},                   // * 17179869184
	{11, "2910383045673370361328125"},                  // * 34359738368
	{11, "14551915228366851806640625"},                 // * 68719476736
	{12, "72759576141834259033203125"},                 // * 137438953472
	{12, "363797880709171295166015625"},                // * 274877906944
	{12, "1818989403545856475830078125"},               // * 549755813888
	{13, "9094947017729282379150390625"},               // * 1099511627776
	{13, "45474735088646411895751953125"},              // * 2199023255552
	{13, "227373675443232059478759765625"},             // * 4398046511104
	{13, "1136868377216160297393798828125"},            // * 8796093022208
	{14, "5684341886080801486968994140625"},            // * 17592186044416
	{14, "28421709430404007434844970703125"},           // * 35184372088832
	{14, "142108547152020037174224853515625"},          // * 70368744177664
	{15, "710542735760100185871124267578125"},          // * 140737488355328
	{15, "3552713678800500929355621337890625"},         // * 281474976710656
	{15, "17763568394002504646778106689453125"},        // * 562949953421312
	{16, "88817841970012523233890533447265625"},        // * 1125899906842624
	{16, "444089209850062616169452667236328125"},       // * 2251799813685248
	{16, "2220446049250313080847263336181640625"},      // * 4503599627370496
	{16, "11102230246251565404236316680908203125"},     // * 9007199254740992
	{17, "55511151231257827021181583404541015625"},     // * 18014398509481984
	{17, "277555756156289135105907917022705078125"},    // * 36028797018963968
	{17, "1387778780781445675529539585113525390625"},   // * 72057594037927936
	{18, "6938893903907228377647697925567626953125"},   // * 144115188075855872
	{18, "34694469519536141888238489627838134765625"},  // * 288230376151711744
	{18, "173472347597680709441192448139190673828125"}, // * 576460752303423488
	{19, "867361737988403547205962240695953369140625"}, // * 1152921504606846976
]

impl Decimal
{
    // If we chop a at nd digits, should we round up?
    func shouldRoundUp(using self, nd1: s64)->bool
    {
        if nd1 < 0 || nd1 >= nd
            return false
        if d[nd1] == "5"'u8 && nd1 + 1 == nd  // exactly halfway - round to even
        {
            // if we truncated, a little higher than what's recorded - always round up
            if trunc
                return true
            return nd1 > 0 && (d[nd1-1] -"0"'u8) % 2 != 0
        }

        // not halfway - digit tells all
        return d[nd1] >= "5"'u8
    }

    // Round a to nd digits (or fewer).
    // If nd is zero, it means we're rounding
    // just to the left of the digits, as in
    // 0.09 -> 0.1.
    func round(using self, nd1: s64)
    {
        if nd1 < 0 || nd1 >= nd
            return
        if shouldRoundUp(nd1)
            roundUp(nd1)
        else
            roundDown(nd1)
    }

    // Round a down to nd digits (or fewer).
    func roundDown(using self, nd1: s64)
    {
        if nd1 < 0 || nd1 >= nd
            return
        nd = nd1
        trim()
    }

    // Round a up to nd digits (or fewer).
    func roundUp(using self, nd1: s64)
    {
        if nd1 < 0 || nd1 >= nd
            return
        // round up
        for i := nd1 - 1; i >= 0; i -= 1
        {
            c := d[i]
            if c < "9"'u8 // can stop after this digit
            {
                d[i] += 1
                nd = i + 1
                return
            }
        }

        // Number is all 9s.
        // Change to single 1 with adjusted decimal point.
        d[0] = "1"'u8
        nd = 1
        dp += 1
    }

    func assign(using self, mant: u64)
    {
        var buf: [24] u8

        // Write reversed decimal in buf.
        v := mant
        n := 0
        while v > 0
        {
            v1 := v / 10
            v -= 10 * v1
            buf[n] = cast(u8) (v + "0"'u8)
            n += 1
            v = v1
        }

        // Reverse again to produce forward decimal in a.d.
        nd = 0
        n -= 1
        while n >= 0
        {
            d[nd] = buf[n]
            nd += 1
            n -= 1
        }

        dp = nd
        trim()
    }

    func trim(using self)
    {
        while nd > 0 && d[nd - 1] == "0"'u8
            nd -= 1
        if nd == 0 dp = 0
    }

    const MaxShift: u32 = 64 - 4
    func rightShift(using self, k: u32)
    {
        r := 0 // read pointer
        w := 0 // write pointer

        // Pick up enough leading digits to cover first shift.
        var n: u64
        while n >> k == 0
        {
            if r >= nd
            {
                if n == 0
                {
                    nd = 0
                    return
                }

                while n >> k == 0
                {
                    n = n * 10
                    r += 1
                }

                break
            }

            c := cast(u64) d[r]
            n = (n * 10) + c - "0"'u8
            r += 1
        }

        dp -= r - 1

        var mask: u64 = (1'u64 << k) - 1

        // Pick up a digit, put down a digit.
        while r < nd
        {
            c := cast(u64) d[r]
            dig := n >> k
            n &= mask
            d[w] = cast(u8) (dig + "0"'u8)
            w += 1
            n = (n * 10) + c - "0"'u8
            r += 1
        }

        // Put down extra digits.
        while n > 0
        {
            dig := n >> k
            n &= mask
            if w < @countof(d)
            {
                d[w] = cast(u8) (dig + "0"'u8)
                w += 1
            }
            else if dig > 0
            {
                trunc = true
            }

            n *= 10
        }

        nd = w
        trim()
    }

    func prefixIsLessThan(b: const [..] u8, s: string) -> bool
    {
        for i := 0; i < @countof(s); i += 1
        {
            if i >= @countof(b)
                return true
            if b[i] != s[i]
                return b[i] < s[i]
        }

        return false
    }

    func leftShift(using self, k: u32)
    {
        delta := LeftCheats[k].delta
        if prefixIsLessThan(@mkslice(&d[0], nd), LeftCheats[k].cutoff)
            delta -= 1

        r := nd         // read index
        w := nd + delta // write index

        // Pick up a digit, put down a digit.
        var n: u64
        r -= 1
        while r >= 0
        {
            n += (cast(u64) (d[r]) - "0"'u8) << k
            quo := n / 10
            rem := n - (10 * quo)
            w -= 1
            if w < @countof(d)
                d[w] = cast(u8) (rem + "0"'u8)
            else if rem != 0
                trunc = true
            n = quo
            r -= 1
        }

        // Put down extra digits.
        while n > 0
        {
            quo := n / 10
            rem := n - (10 * quo)
            w -= 1
            if w < @countof(d)
                d[w] = cast(u8) (rem + "0"'u8)
            else if rem != 0
                trunc = true
            n = quo
        }

        nd += delta
        if nd >= @countof(d)
            nd = cast(s32) @countof(d)
        dp += delta
        trim()
    }

    func shift(using self, kk: s32)
    {
        k := kk
        switch
        {
        case nd == 0:
            break

        case k > 0:
            while k > MaxShift
            {
                leftShift(MaxShift)
                k -= MaxShift
            }

            leftShift(cast(u32) k)

        case k < 0:
            while k < -cast(s32)MaxShift
            {
                rightShift(MaxShift)
                k += MaxShift
            }

            rightShift(cast(u32) (-k))
        }
    }

    func readF64(using self, str: const [..] u8)->u32 throw
    {
        cpt := 0'u32
        countof := @countof(str)
        buffer := @dataof(str)

        negSign := g_CultureInfo.numberFormat.negativeSign
        plusSign := g_CultureInfo.numberFormat.positiveSign
        dotSign := g_CultureInfo.numberFormat.decimalSeparator

        c := :buffer
        if c == negSign
        {
            buffer, cpt += 1
            neg = true
        }
        else if c == plusSign
        {
            buffer, cpt += 1
        }

        // digits
        maxMantDigits := 19 // 10^19 fits in u64
        sawdot    := false
        sawDigits := false
        ndMant := 0

        nd = 0
        dp = 0

        while cpt < countof
        {
            c = :buffer
            if c == dotSign
            {
                if sawdot break
                sawdot = true
                dp = nd
            }
            else if "0"'u8 <= c && c <= "9"'u8
            {
                sawDigits = true
                if c == "0"'u8 && nd == 0
                    dp -= 1
                else
                {
                    if nd < @countof(d)
                    {
                        d[nd] = c
                        nd += 1

                        if ndMant < maxMantDigits
                        {
                            mantissa *= 10
                            mantissa += cast(u64) (c - "0"'u8)
                            ndMant += 1
                        }
                        else if c != "0"'u8
                            trunc = true
                    }
                    else if c != "0"'u8
                        bigtrunc = true
                }
            }
            else
                break

            cpt, buffer += 1
        }

        if !sawDigits
            throw "invalid float format"
        if !sawdot
            dp = nd

        // Exponent
        c = :buffer
        if cpt < countof && (c|32) == "e"'u8
        {
            cpt, buffer += 1
            if cpt >= countof
                throw "invalid float format"

            esign := 1
            c = :buffer
            if c == negSign
            {
                buffer, cpt += 1
                esign = -1
                c = :buffer
            }
            else if c == plusSign
            {
                buffer, cpt += 1
                c = :buffer
            }

            if cpt >= countof || c < "0"'u8 || c > "9"'u8
                throw "invalid float format"

            e := 0
            while cpt < countof
            {
                c = :buffer
                if c < "0"'u8 || c > "9"'u8
                    break
                if e < 10000
                    e = e*10 + (c - "0"'u8)
                buffer, cpt += 1
            }

            dp += e * esign
        }

        if mantissa != 0
            exp = dp - ndMant

        return cpt
    }
}