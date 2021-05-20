//https://github.com/golang/go/blob/master/src/strconv/extfloat.go

#global namespace strconv
#global #[Swag.safety("overflow", false)]

// An extFloat represents an extended floating-point number, with more
// precision than a float64. It does not try to save bits: the
// number represented by the structure is mant*(2^exp), with a negative
// sign if neg is true.
struct FloatExt
{
	mant: u64
	exp:  s64
	neg:  bool
}

// Powers of ten taken from double-conversion library.
// https://code.google.com/p/double-conversion/
const firstPowerOfTen = -348
const stepPowerOfTen  = 8

var SmallPowersOfTen: [] FloatExt = @[
	{1'u64 << 63, -63, false},        // 1
	{0xa'u64 << 60, -60, false},      // 1e1
	{0x64'u64 << 57, -57, false},     // 1e2
	{0x3e8'u64 << 54, -54, false},    // 1e3
	{0x2710'u64 << 50, -50, false},   // 1e4
	{0x186a0'u64 << 47, -47, false},  // 1e5
	{0xf4240'u64 << 44, -44, false},  // 1e6
	{0x989680'u64 << 40, -40, false}, // 1e7
]

var powersOfTen: [] FloatExt = @[
	{0xfa8fd5a0081c0288, -1220, false}, // 10^-348
	{0xbaaee17fa23ebf76, -1193, false}, // 10^-340
	{0x8b16fb203055ac76, -1166, false}, // 10^-332
	{0xcf42894a5dce35ea, -1140, false}, // 10^-324
	{0x9a6bb0aa55653b2d, -1113, false}, // 10^-316
	{0xe61acf033d1a45df, -1087, false}, // 10^-308
	{0xab70fe17c79ac6ca, -1060, false}, // 10^-300
	{0xff77b1fcbebcdc4f, -1034, false}, // 10^-292
	{0xbe5691ef416bd60c, -1007, false}, // 10^-284
	{0x8dd01fad907ffc3c, -980, false},  // 10^-276
	{0xd3515c2831559a83, -954, false},  // 10^-268
	{0x9d71ac8fada6c9b5, -927, false},  // 10^-260
	{0xea9c227723ee8bcb, -901, false},  // 10^-252
	{0xaecc49914078536d, -874, false},  // 10^-244
	{0x823c12795db6ce57, -847, false},  // 10^-236
	{0xc21094364dfb5637, -821, false},  // 10^-228
	{0x9096ea6f3848984f, -794, false},  // 10^-220
	{0xd77485cb25823ac7, -768, false},  // 10^-212
	{0xa086cfcd97bf97f4, -741, false},  // 10^-204
	{0xef340a98172aace5, -715, false},  // 10^-196
	{0xb23867fb2a35b28e, -688, false},  // 10^-188
	{0x84c8d4dfd2c63f3b, -661, false},  // 10^-180
	{0xc5dd44271ad3cdba, -635, false},  // 10^-172
	{0x936b9fcebb25c996, -608, false},  // 10^-164
	{0xdbac6c247d62a584, -582, false},  // 10^-156
	{0xa3ab66580d5fdaf6, -555, false},  // 10^-148
	{0xf3e2f893dec3f126, -529, false},  // 10^-140
	{0xb5b5ada8aaff80b8, -502, false},  // 10^-132
	{0x87625f056c7c4a8b, -475, false},  // 10^-124
	{0xc9bcff6034c13053, -449, false},  // 10^-116
	{0x964e858c91ba2655, -422, false},  // 10^-108
	{0xdff9772470297ebd, -396, false},  // 10^-100
	{0xa6dfbd9fb8e5b88f, -369, false},  // 10^-92
	{0xf8a95fcf88747d94, -343, false},  // 10^-84
	{0xb94470938fa89bcf, -316, false},  // 10^-76
	{0x8a08f0f8bf0f156b, -289, false},  // 10^-68
	{0xcdb02555653131b6, -263, false},  // 10^-60
	{0x993fe2c6d07b7fac, -236, false},  // 10^-52
	{0xe45c10c42a2b3b06, -210, false},  // 10^-44
	{0xaa242499697392d3, -183, false},  // 10^-36
	{0xfd87b5f28300ca0e, -157, false},  // 10^-28
	{0xbce5086492111aeb, -130, false},  // 10^-20
	{0x8cbccc096f5088cc, -103, false},  // 10^-12
	{0xd1b71758e219652c, -77, false},   // 10^-4
	{0x9c40000000000000, -50, false},   // 10^4
	{0xe8d4a51000000000, -24, false},   // 10^12
	{0xad78ebc5ac620000, 3, false},     // 10^20
	{0x813f3978f8940984, 30, false},    // 10^28
	{0xc097ce7bc90715b3, 56, false},    // 10^36
	{0x8f7e32ce7bea5c70, 83, false},    // 10^44
	{0xd5d238a4abe98068, 109, false},   // 10^52
	{0x9f4f2726179a2245, 136, false},   // 10^60
	{0xed63a231d4c4fb27, 162, false},   // 10^68
	{0xb0de65388cc8ada8, 189, false},   // 10^76
	{0x83c7088e1aab65db, 216, false},   // 10^84
	{0xc45d1df942711d9a, 242, false},   // 10^92
	{0x924d692ca61be758, 269, false},   // 10^100
	{0xda01ee641a708dea, 295, false},   // 10^108
	{0xa26da3999aef774a, 322, false},   // 10^116
	{0xf209787bb47d6b85, 348, false},   // 10^124
	{0xb454e4a179dd1877, 375, false},   // 10^132
	{0x865b86925b9bc5c2, 402, false},   // 10^140
	{0xc83553c5c8965d3d, 428, false},   // 10^148
	{0x952ab45cfa97a0b3, 455, false},   // 10^156
	{0xde469fbd99a05fe3, 481, false},   // 10^164
	{0xa59bc234db398c25, 508, false},   // 10^172
	{0xf6c69a72a3989f5c, 534, false},   // 10^180
	{0xb7dcbf5354e9bece, 561, false},   // 10^188
	{0x88fcf317f22241e2, 588, false},   // 10^196
	{0xcc20ce9bd35c78a5, 614, false},   // 10^204
	{0x98165af37b2153df, 641, false},   // 10^212
	{0xe2a0b5dc971f303a, 667, false},   // 10^220
	{0xa8d9d1535ce3b396, 694, false},   // 10^228
	{0xfb9b7cd9a4a7443c, 720, false},   // 10^236
	{0xbb764c4ca7a44410, 747, false},   // 10^244
	{0x8bab8eefb6409c1a, 774, false},   // 10^252
	{0xd01fef10a657842c, 800, false},   // 10^260
	{0x9b10a4e5e9913129, 827, false},   // 10^268
	{0xe7109bfba19c0c9d, 853, false},   // 10^276
	{0xac2820d9623bf429, 880, false},   // 10^284
	{0x80444b5e7aa7cf85, 907, false},   // 10^292
	{0xbf21e44003acdd2d, 933, false},   // 10^300
	{0x8e679c2f5e44ff8f, 960, false},   // 10^308
	{0xd433179d9c8cb841, 986, false},   // 10^316
	{0x9e19db92b4e31ba9, 1013, false},  // 10^324
	{0xeb96bf6ebadf77d9, 1039, false},  // 10^332
	{0xaf87023b9bf0ee6b, 1066, false},  // 10^340
]

impl FloatExt
{
    // AssignComputeBounds sets f to the floating point value
    // defined by mant, exp and precision given by flt. It returns
    // lower, upper such that any number in the closed interval
    // [lower, upper] is converted back to the same floating point number.
    func assignComputeBounds(using self, mantp: u64, expp: int, negp: bool)->{lower: FloatExt, upper: FloatExt}
    {
        var result: retval

        mant = mantp
        exp = expp - Swag.F64.MantBits
        neg = negp
        if exp <= 0 and mant == (mant >> cast(u32) (-exp)) << cast(u32)(-exp)
        {
            // An exact integer
            mant >>= cast(u32) (-exp)
            exp = 0
            result.lower = dref self
            result.upper = dref self
            return result
        }

        expBiased := exp - Swag.F64.Bias

        result.upper = FloatExt{2*mant + 1, exp - 1, neg}
        if mant != 1'u64 << Swag.F64.MantBits or expBiased == 1
            result.lower = FloatExt{2*mant - 1, exp - 1, neg}
        else
            result.lower = FloatExt{4*mant - 1, exp - 2, neg}
        return result
    }

    // Normalize normalizes f so that the highest bit of the mantissa is
    // set, and returns the number by which the mantissa was left-shifted.
    func normalize(using self)
    {
        if mant == 0 return
        shift := @bitcountlz(mant)
        mant <<= cast(u32) shift
        exp -= cast(s64) shift
    }

    // Multiply sets f to the product f*g: the result is correctly rounded,
    // but not normalized.
    func multiply(using self, g: FloatExt)
    {
        (hi, lo) := Math.mul64(mant, g.mant)
        // Round up.
        mant = hi + (lo >> 63)
        exp = exp + g.exp + 64
    }

    const uint64pow10: [] u64 = @[
        1,
        10,
        100,
        1000,
        10000,
        100000,
        1000000,
        10000000,
        100000000,
        1000000000,
        10000000000,
        100000000000,
        1000000000000,
        10000000000000,
        100000000000000,
        1000000000000000,
        10000000000000000,
        100000000000000000,
        1000000000000000000,
        10000000000000000000,
    ]

    // Frexp10 is an analogue of Math.Frexp for decimal powers. It scales
    // f by an approximate power of ten 10^-exp, and returns exp10, so
    // that f*10^exp10 has the same value as the old f, up to an ulp,
    // as well as the index of 10^-exp in the powersOfTen table.
    func frexp10(using self)->{exp10: s64, index: s64}
    {
        // The constants expMin and expMax constrain the final value of the
        // binary exponent of f. We want a small integral part in the result
        // because finding digits of an integer requires divisions, whereas
        // digits of the fractional part can be found by repeatedly multiplying
        // by 10.
        const expMin = -60
        const expMax = -32

        // Find power of ten such that x * 10^n has a binary exponent
        // between expMin and expMax.
        approxExp10 := ((expMin+expMax)/2 - exp) * 28 / 93 // log(10)/log(2) is close to 93/28.
        i := (approxExp10 - firstPowerOfTen) / stepPowerOfTen
        label Loop loop
        {
            exp1 := exp + powersOfTen[i].exp + 64
            switch
            {
            case exp1 < expMin:
                i += 1
            case exp1 > expMax:
                i -= 1
            default:
                break Loop
            }
        }

        // Apply the desired decimal shift on f. It will have exponent
        // in the desired range. This is multiplication by 10^-exp10.
        multiply(powersOfTen[i])

        var result: retval
        result.exp10 = -(firstPowerOfTen + i*stepPowerOfTen)
        result.index = i
        return result
    }

    // frexp10Many applies a common shift by a power of ten to a, b, c.
    func frexp10Many(a, b, c: *FloatExt)->s64
    {
        (exp10, i) := c.frexp10()
        a.multiply(powersOfTen[i])
        b.multiply(powersOfTen[i])
        return exp10
    }

    // FixedDecimal stores in d the first n significant digits
    // of the decimal representation of f. It returns false
    // if it cannot be sure of the answer.
    func fixedDecimal(using self, d: *DecimalSlice, n: s64)->bool
    {
        if mant == 0
        {
            d.nd = 0
            d.dp = 0
            d.neg = neg
            return true
        }

        // Multiply by an appropriate power of ten to have a reasonable
        // number to process.
        normalize()
        (exp10, ?) := frexp10()

        shift := cast(u32) -exp
        integer := cast(u32) (mant >> shift)
        fraction := mant - (cast(u64) integer << shift)
        e := 1'u64 // e is the uncertainty we have on the mantissa of f.

        // Write exactly n digits to d.
        needed := n        // how many digits are left to write.
        integerDigits := 0 // the number of decimal digits of integer.
        pow10 := 1'u64     // the power of ten by which f was scaled.

        pow := 1'u64
        loop i: 20
        {
            if pow > cast(u64) integer
            {
                integerDigits = i
                break
            }

            pow *= 10
        }

        rest := integer
        if integerDigits > needed
        {
            // the integral part is already large, trim the last digits.
            pow10 = uint64pow10[integerDigits-needed]
            integer /= cast(u32) pow10
            rest -= integer * cast(u32) pow10
        }
        else
            rest = 0

        // Write the digits of integer: the digits of rest are omitted.
        var buf: [32] u8
        pos := @countof(buf)

        v := integer
        while v > 0
        {
            v1 := v / 10
            v -= 10 * v1
            pos -= 1
            buf[pos] = cast(u8) (v + "0"'u8)
            v = v1
        }

        for i := pos; i < @countof(buf); i += 1
            d.d[i-pos] = buf[i]

        nd := cast(s64) (@countof(buf) - pos)
        d.nd = nd
        d.dp = integerDigits + exp10
        needed -= nd

        if needed > 0
        {
            // Emit digits for the fractional part. Each time, 10*fraction
            // fits in a uint64 without overflow.
            while needed > 0
            {
                fraction *= 10
                e *= 10 // the uncertainty scales as we multiply by ten.
                if 2*e > (1<<shift)
                {
                    // the error is so large it could modify which digit to write, abort.
                    return false
                }

                digit := fraction >> shift
                d.d[nd] = cast(u8) (digit + "0"'u8)
                fraction -= digit << shift
                nd += 1
                needed -= 1
            }
            d.nd = nd
        }

        // We have written a truncation of f (a numerator / 10^d.dp). The remaining part
        // can be interpreted as a small number (< 1) to be added to the last digit of the
        // numerator.
        //
        // If rest > 0, the amount is:
        //    (rest<<shift | fraction) / (pow10 << shift)
        //    fraction being known with a ±e uncertainty.
        //    The fact that n > 0 guarantees that pow10 << shift does not overflow a uint64.
        //
        // If rest = 0, pow10 == 1 and the amount is
        //    fraction / (1 << shift)
        //    fraction being known with a ±e uncertainty.
        //
        // We pass this information to the rounding routine for adjustment.

        ok := adjustLastDigitFixed(d, (cast(u64)(rest) << shift) | fraction, pow10, shift, e)
        if !ok
            return false

        // Trim trailing zeros.
        for i := d.nd - 1; i >= 0; i -= 1
        {
            if d.d[i] != "0"'u8
            {
                d.nd = i + 1
                break
            }
        }

        return true
    }

    // adjustLastDigitFixed assumes d contains the representation of the integral part
    // of some number, whose fractional part is num / (den << shift). The numerator
    // num is only known up to an uncertainty of size e, assumed to be less than
    // (den << shift)/2.
    //
    // It will increase the last digit by one to account for correct rounding, typically
    // when the fractional part is greater than 1/2, and will return false if e is such
    // that no correct answer can be given.
    func adjustLastDigitFixed(d: *DecimalSlice, num, den: u64, shift: u32, e: u64)->bool
    {
        if 2*(num+e) < (den << shift)
            return true

        if 2*(num-e) > (den << shift)
        {
            // increment d by 1.
            i := d.nd - 1
            while i >= 0
            {
                if d.d[i] == "9"'u8
                    d.nd -= 1
                else
                    break
                i -= 1
            }

            if i < 0
            {
                d.d[0] = "1"'u8
                d.nd = 1
                d.dp += 1
            }
            else
                d.d[i] += 1
            return true
        }

        return false
    }

    func opEquals(using self, other: Self)->bool
    {
        return mant == other.mant and exp == other.exp and neg == other.neg
    }

    // ShortestDecimal stores in d the shortest decimal representation of f
    // which belongs to the open interval (lower, upper), where f is supposed
    // to lie. It returns false whenever the result is unsure. The implementation
    // uses the Grisu3 algorithm.
    func shortestDecimal(using self, d: *DecimalSlice, lower, upper: *FloatExt)->bool
    {
        if mant == 0
        {
            d.nd = 0
            d.dp = 0
            d.neg = neg
            return true
        }

        if exp == 0 and dref lower == dref self and dref lower == dref upper
        {
            // an exact integer.
            var buf: [24] u8
            n := @countof(buf) - 1
            v := mant
            while v > 0
            {
                v1 := v / 10
                v -= 10 * v1
                buf[n] = cast(u8) (v + "0"'u8)
                n -= 1
                v = v1
            }

            nd := cast(s64) (@countof(buf) - n - 1)
            for i := 0; i < nd; i += 1
                d.d[i] = buf[n+1+i]

            d.nd = nd
            d.dp = nd
            while d.nd > 0 and d.d[d.nd-1] == "0"'u8
                d.nd -= 1

            if d.nd == 0 d.dp = 0
            d.neg = neg
            return true
        }

        upper.normalize()

        // Uniformize exponents.
        if exp > upper.exp
        {
            mant <<= cast(u32) (exp - upper.exp)
            exp = upper.exp
        }

        if lower.exp > upper.exp
        {
            lower.mant <<= cast(u32) (lower.exp - upper.exp)
            lower.exp = upper.exp
        }

        exp10 := frexp10Many(lower, self, upper)

        // Take a safety margin due to rounding in frexp10Many, but we lose precision.
        upper.mant += 1
        lower.mant -= 1

        // The shortest representation of f is either rounded up or down, but
        // in any case, it is a truncation of upper.
        shift := cast(u32) -upper.exp
        integer := cast(u32) (upper.mant >> shift)
        fraction := upper.mant - ((cast(u64) integer) << shift)

        // How far we can go down from upper until the result is wrong.
        allowance := upper.mant - lower.mant

        // How far we should go to get a very precise result.
        targetDiff := upper.mant - mant

        // Count integral digits: there are at most 10.
        var integerDigits: s64
        pow := 1'u64
        loop i: 20
        {
            if pow > cast(u64) integer
            {
                integerDigits = i
                break
            }

            pow *= 10
        }

        loop i: integerDigits
        {
            pow = uint64pow10[integerDigits-i-1]
            digit := integer / cast(u32) pow
            d.d[i] = cast(u8) (digit + "0"'u8)
            integer -= digit * cast(u32) pow

            // evaluate whether we should stop.
            currentDiff := (cast(u64) integer << shift) + fraction
            if currentDiff < allowance
            {
                d.nd = i + 1
                d.dp = integerDigits + exp10
                d.neg = neg
                // Sometimes allowance is so large the last digit might need to be
                // decremented to get closer to f.
                return adjustLastDigit(d, currentDiff, targetDiff, allowance, pow << shift, 2)
            }
        }

        d.nd = integerDigits
        d.dp = d.nd + exp10
        d.neg = neg

        // Compute digits of the fractional part. At each step fraction does not
        // overflow. The choice of minExp implies that fraction is less than 2^60.
        var digit: s64
        multiplier := 1'u64
        loop
        {
            fraction *= 10
            multiplier *= 10
            digit = cast(s64) (fraction >> shift)
            d.d[d.nd] = cast(u8) (digit + "0"'u8)
            d.nd += 1
            fraction -= cast(u64) digit << shift
            if fraction < allowance * multiplier
            {
                // We are in the admissible range. Note that if allowance is about to
                // overflow, that is, allowance > 2^64/10, the condition is automatically
                // true due to the limited range of fraction.
                return adjustLastDigit(d, fraction, targetDiff*multiplier, allowance*multiplier, 1'u64 << shift, multiplier*2)
            }
        }

        return false
    }

    // adjustLastDigit modifies d = x-currentDiff*ε, to get closest to
    // d = x-targetDiff*ε, without becoming smaller than x-maxDiff*ε.
    // It assumes that a decimal digit is worth ulpDecimal*ε, and that
    // all data is known with an error estimate of ulpBinary*ε.
    func adjustLastDigit(d: *DecimalSlice, cdiff, targetDiff, maxDiff, ulpDecimal, ulpBinary: u64)->bool
    {
        currentDiff := cdiff

        // Approximation is too wide.
        if ulpDecimal < 2*ulpBinary
            return false

        while currentDiff + (ulpDecimal / 2) + ulpBinary < targetDiff
        {
            d.d[d.nd-1] -= 1
            currentDiff += ulpDecimal
        }

        // we have two choices, and don't know what to do.
        if currentDiff + ulpDecimal <= targetDiff + (ulpDecimal / 2) + ulpBinary
            return false

        // we went too far
        if currentDiff < ulpBinary or currentDiff > maxDiff - ulpBinary
            return false

        // the number has actually reached zero.
        if d.nd == 1 and d.d[0] == "0"'u8
        {
            d.nd = 0
            d.dp = 0
        }

        return true
    }
}