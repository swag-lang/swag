#global skip
// https://github.com/golang/go/blob/master/src/strconv/atof.go

struct FloatInfo
{
	mantbits: u32
	expbits:  u32
	bias:     s32
}

const float32info: FloatInfo = @{23, 8, -127}
const float64info: FloatInfo = @{52, 11, -1023}

func parseF64Exact(mantissa: u64, exponent: s32, neg: bool) -> {f: f64, ok: bool}
{
    const Float64Pow10: [] f64 = @[
        1e0, 1e1, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9,
        1e10, 1e11, 1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19,
        1e20, 1e21, 1e22,
    ]


    var result: retval

	if mantissa >> float64info.mantbits != 0
     	return result

    exp := exponent
	f := cast(f64) mantissa
	if neg f = -f

	switch
    {
    // Integer
	case exp == 0:
        result.f = f
        result.ok = true
		return result

	// Exact integers are <= 10^15.
	// Exact powers of ten are <= 10^22.
	case exp > 0 && exp <= 15+22:
		// If exponent is big but number of digits is not, can move a few zeros
        // into the integer part.
		if exp > 22
        {
			f *= Float64Pow10[exp-22]
			exp = 22
		}

        // The exponent was really too large.
		if f > 1e15 || f < -1e15
			return result

        result.f = f * Float64Pow10[exp]
        result.ok = true
		return result

    // Integer / 10^k
	case exp < 0 && exp >= -22:
		result.f = f / Float64Pow10[-exp]
        result.ok = true
        return result
	}

    return result
}

func readFloat(str: string)->{mantissa: u64, exp: s32, neg: bool, trunc: bool, eat: s32} throw
{
    var result: retval

    cpt := 0
    countof := @countof(str)
    buffer := @dataof(str)

    c := :buffer
    if c == "-"'u8 //g_CultureInfo.numberFormat.negativeSign
    {
        buffer, cpt += 1
        result.neg = true
    }
    else if c == "+"'u8 //g_CultureInfo.numberFormat.positiveSign
    {
        buffer, cpt += 1
    }

	// digits
	maxMantDigits := 19 // 10^19 fits in u64
	sawdot    := false
	sawdigits := false
	nd := 0
	ndMant := 0
	dp := 0

    while cpt < countof
    {
        c = :buffer
		if c == "."'u8
        {
			if sawdot break
			sawdot = true
			dp = nd
        }
        else if "0"'u8 <= c && c <= "9"'u8
        {
			sawdigits = true
			if c == "0"'u8 && nd == 0 // ignore leading zeros
				dp -= 1
            else
            {
                nd += 1
                if ndMant < maxMantDigits
                {
                    result.mantissa *= 10
                    result.mantissa += cast(u64) (c - "0"'u8)
                    ndMant += 1
                }
                else if c != "0"'u8
                {
                    result.trunc = true
                }
            }
		}
        else
            break

        cpt, buffer += 1
	}

	if !sawdigits
		return result
	if !sawdot
		dp = nd

	// optional exponent moves decimal point.
	// if we read a very large, very long number,
	// just be sure to move the decimal point by
	// a lot (say, 100000).  it doesn't matter if it's
	// not the exact number.
    c = :buffer
	if cpt < countof && (c|32) == "e"'u8
    {
		cpt, buffer += 1
		if cpt >= countof
			throw "invalid"

		esign := 1
        if c == "-"'u8 //g_CultureInfo.numberFormat.negativeSign
        {
            buffer, cpt += 1
            esign = -1
        }
        else if c == "+"'u8 //g_CultureInfo.numberFormat.positiveSign
        {
            buffer, cpt += 1
        }

        c = :buffer
		if cpt >= countof || c < "0"'u8 || c > "9"'u8
			throw "invalid"

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

	if result.mantissa != 0
		result.exp = dp - ndMant
    return result
}

func atof64(s: string)-> { f64, eat: u32 }
{
    var result: retval
    countof := @countof(s)
    if !countof return result

	/*if val, n, ok := special(s); ok {
		return val, n, nil
	}*/

	(mantissa, exp, neg, trunc, n) := assume readFloat(s)

/*	if optimize {
		// Try pure floating-point arithmetic conversion, and if that fails,
		// the Eisel-Lemire algorithm.
		if !trunc {
			if f, ok := atof64exact(mantissa, exp, neg); ok {
				return f, n, nil
			}
		}
		f, ok := eiselLemire64(mantissa, exp, neg)
		if ok {
			if !trunc {
				return f, n, nil
			}
			// Even if the mantissa was truncated, we may
			// have found the correct result. Confirm by
			// converting the upper mantissa bound.
			fUp, ok := eiselLemire64(mantissa+1, exp, neg)
			if ok && f == fUp {
				return f, n, nil
			}
		}
	}*/

	// Slow fallback.
	/*var d decimal
	if !d.set(s[:n]) {
		return 0, n, syntaxError(fnParseFloat, s)
	}
	b, ovf := d.floatBits(&float64info)
	f = math.Float64frombits(b)
	if ovf {
		err = rangeError(fnParseFloat, s)
	}
	return f, n, err*/
    return result
}