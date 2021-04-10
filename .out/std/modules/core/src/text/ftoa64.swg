// https://github.com/golang/go/blob/11f159456b1dba3ec499da916852dd188d1e04a7/src/strconv/ftoa.go

#global namespace strconv
#global private
#global #[swag.safety("of", false)]

struct leftCheat
{
	delta:  s64    // number of new digits
	cutoff: string // minus one digit if original < a.
}

var leftcheats: []leftCheat = @[
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

struct FloatInfo
{
	mantbits: u32
	expbits:  u32
	bias:     s32
}

struct Decimal
{
	d:     [800] u8  // digits, big-endian representation
	nd:    s64       // number of digits used
	dp:    s64       // decimal point
	neg:   bool      // negative flag
	trunc: bool      // discarded nonzero digits beyond d[:nd]
}

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

    const maxShift: u32 = 64 - 4

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
        delta := leftcheats[k].delta
        if prefixIsLessThan(@mkslice(&d[0], nd), leftcheats[k].cutoff)
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
            while k > maxShift
            {
                leftShift(maxShift)
                k -= maxShift
            }

            leftShift(cast(u32) k)

        case k < 0:
            while k < -cast(s32)maxShift
            {
                rightShift(maxShift)
                k += maxShift
            }

            rightShift(cast(u32) (-k))
        }
    }
}

const infoF64: FloatInfo = @{52, 11, -1023}

// The format fmt is one of
// 'e' (-d.dddde±dd, a decimal exponent),
// 'E' (-d.ddddE±dd, a decimal exponent),
// 'f' (-ddd.dddd, no exponent),
// 'g' ('e' for large exponents, 'f' otherwise),
// 'G' ('E' for large exponents, 'f' otherwise),

public func f64ToA(buf: *ConcatBuffer, val: f64, fmt: u8, prec: s64)
{
	var bits: u64 = bitcast(u64) val

	neg  := bits >> (infoF64.expbits + infoF64.mantbits) != 0
	exp  := cast(s64) (bits >> infoF64.mantbits) & ((1 << infoF64.expbits) - 1)
	mant := bits & ((1'u64 << infoF64.mantbits) - 1)

	switch exp
    {
    // Inf, NaN
	case (1 << infoF64.expbits) - 1:
		var s: string
		switch
        {
		case mant != 0: s = "nan"
		case neg:       s = "-inf"
		default:        s = "inf"
		}

		buf.addBytes(s)
        return

	case 0:
		exp += 1

	default:
		mant |= 1'u64 << infoF64.mantbits
	}

	exp += infoF64.bias

/*	var digs decimalSlice
	ok := false
	// Negative precision means "only as much as needed to be exact."
	shortest := prec < 0
	if shortest {
		// Try Grisu3 algorithm.
		f := new(extFloat)
		lower, upper := f.AssignComputeBounds(mant, exp, neg, flt)
		var buf [32]byte
		digs.d = buf[:]
		ok = f.ShortestDecimal(&digs, &lower, &upper)
		if !ok {
			return bigFtoa(dst, prec, fmt, neg, mant, exp, flt)
		}
		// Precision for shortest representation mode.
		switch fmt {
		case 'e', 'E':
			prec = max(digs.nd-1, 0)
		case 'f':
			prec = max(digs.nd-digs.dp, 0)
		case 'g', 'G':
			prec = digs.nd
		}
	} else if fmt != 'f' {
		// Fixed number of digits.
		digits := prec
		switch fmt {
		case 'e', 'E':
			digits++
		case 'g', 'G':
			if prec == 0 {
				prec = 1
			}
			digits = prec
		}
		if digits <= 15 {
			// try fast algorithm when the number of digits is reasonable.
			var buf [24]byte
			digs.d = buf[:]
			f := extFloat{mant, exp - int(flt.mantbits), neg}
			ok = f.FixedDecimal(&digs, digits)
		}
	}
	if !ok {
		return bigFtoa(dst, prec, fmt, neg, mant, exp, flt)
	}
	return formatDigits(dst, shortest, neg, digs, prec, fmt)*/
    bigFtoa(buf, prec, fmt, neg, mant, exp)
}

struct decimalSlice
{
	d:      [..] u8
	nd, dp: s64
	neg:    bool
}

func bigFtoa(dst: *ConcatBuffer, precision: s64, fmt: u8, neg: bool, mant: u64, exp: s64)
{
    prec := precision

	var d: Decimal
	d.assign(mant)
	d.shift(cast(s32) exp - cast(s32) infoF64.mantbits)

	var digs: decimalSlice
	shortest := prec < 0
	if shortest
    {
		roundShortest(&d, mant, exp)
		digs.d = d.d
        digs.nd = d.nd
        digs.dp = d.dp
		// Precision for shortest representation mode.
		switch fmt
        {
		case "e"'u8, "E"'u8:
			prec = digs.nd - 1
		case "f"'u8:
			prec = math.max(digs.nd-digs.dp, 0)
		case "g"'u8, "G"'u8:
			prec = digs.nd
		}
	}
    else
    {
		// Round appropriately.
		switch fmt
        {
		case "e"'u8, "E"'u8:
			d.round(prec + 1)
		case "f"'u8:
			d.round(d.dp + prec)
		case "g"'u8, "G"'u8:
			d.round(prec == 0 ? 1 : prec)
		}

        digs.d = d.d
        digs.nd = d.nd
        digs.dp = d.dp
	}

	formatDigits(dst, shortest, neg, digs, prec, fmt)
}

func formatDigits(dst: *ConcatBuffer, shortest: bool, neg: bool, digs: decimalSlice, precision: s64, fmt: u8)
{
    prec := precision
	switch fmt
    {
	case "e"'u8, "E"'u8:
		return fmtE(dst, neg, digs, prec, fmt)

	case "f"'u8:
		return fmtF(dst, neg, digs, prec)

	case "g"'u8, "G"'u8:
		// trailing fractional zeros in 'e' form will be trimmed.
		eprec := prec
		if eprec > digs.nd && digs.nd >= digs.dp
			eprec = digs.nd
		// %e is used if the exponent from the conversion
		// is less than -4 or greater than or equal to the precision.
		// if precision was the shortest possible, use precision 6 for this decision.
		if shortest eprec = 6
		exp := digs.dp - 1
		if exp < -4 || exp >= eprec
        {
			if prec > digs.nd
                prec = digs.nd
			return fmtE(dst, neg, digs, prec - 1, cast(u8) (fmt + "e"'u8 - "g"'u8))
		}

		if prec > digs.dp
            prec = digs.nd
		return fmtF(dst, neg, digs, math.max(prec - digs.dp, 0))
	}
}

func fmtE(dst: *ConcatBuffer, neg: bool, d: decimalSlice, prec: s64, fmt: u8)
{
	// sign
	if neg
		dst.addByte("-"'u8)

	// first digit
	ch := "0"'u8
	if d.nd != 0
		ch = d.d[0]
	dst.addByte(ch)

	// .moredigits
	if prec > 0
    {
		dst.addByte("."'u8)
		i := 1's64
		m := math.min(d.nd, prec + 1)
		if i < m
        {
			dst.addBytes(@mkslice(&d.d[i], m - i + 1))
			i = m
		}

		while i <= prec
        {
			dst.addBytes("0")
            i += 1
		}
	}

	// e±
	dst.addByte(fmt)
	exp := d.dp - 1
	if d.nd == 0 // special case: 0 has exponent 0
		exp = 0
	if exp < 0
    {
		ch = "-"'u8
		exp = -exp
	}
    else
    {
		ch = "+"'u8
	}

	dst.addByte(ch)

	// dd or ddd
	switch
    {
	case exp < 10:
		dst.addByte("0"'u8)
        dst.addByte(cast(u8) (exp + "0"'u8))
	case exp < 100:
        dst.addByte(cast(u8) ((exp/10) + "0"'u8))
        dst.addByte(cast(u8) ((exp/100) + "0"'u8))
	default:
        dst.addByte(cast(u8) ((exp/100) + "0"'u8))
        dst.addByte(cast(u8) (((exp/10)%10) + "0"'u8))
        dst.addByte(cast(u8) ((exp%10) + "0"'u8))
	}
}

// %f: -ddddddd.ddddd
func fmtF(dst: *ConcatBuffer, neg: bool, d: decimalSlice, prec: s64)
{
	// sign
	if neg
		dst.addByte("-"'u8)

	// integer, padded with zeros as needed.
	if d.dp > 0
    {
		m := math.min(d.nd, d.dp)
		dst.addBytes(@mkslice(&d.d[0], m))
		while m < d.dp
        {
			dst.addByte("0"'u8)
            m += 1
		}
	}
    else
		dst.addByte("0"'u8)

	// fraction
	if prec > 0
    {
		dst.addByte("."'u8)
		for i := 0; i < prec; i += 1
        {
			ch := "0"'u8
            j := d.dp + i
			if 0 <= j && j < d.nd
				ch = d.d[j]
			dst.addByte(ch)
		}
	}
}

// roundShortest rounds d (= mant * 2^exp) to the shortest number of digits
// that will let the original floating point value be precisely reconstructed.
func roundShortest(d: *Decimal, mant: u64, exp: s64)
{
	// If mantissa is zero, the number is zero; stop now.
	if mant == 0
    {
		d.nd = 0
		return
	}

	// Compute upper and lower such that any decimal number
	// between upper and lower (possibly inclusive)
	// will round to the original floating point number.

	// We may see at once that the number is already shortest.
	//
	// Suppose d is not denormal, so that 2^exp <= d < 10^dp.
	// The closest shorter number is at least 10^(dp-nd) away.
	// The lower/upper bounds computed below are at distance
	// at most 2^(exp-mantbits).
	//
	// So the number is already shortest if 10^(dp-nd) > 2^(exp-mantbits),
	// or equivalently log2(10)*(dp-nd) > exp-mantbits.
	// It is true if 332/100*(dp-nd) >= exp-mantbits (log2(10) > 3.32).
	minexp := infoF64.bias + 1 // minimum possible exponent
	if exp > minexp && 332*(d.dp-d.nd) >= 100*(exp-cast(s64)(infoF64.mantbits))
    {
		// The number is already shortest.
		return
	}

	// d = mant << (exp - mantbits)
	// Next highest floating point number is mant+1 << exp-mantbits.
	// Our upper bound is halfway between, mant*2+1 << exp-mantbits-1.
	var upper: Decimal
	upper.assign(mant*2 + 1)
	upper.shift(cast(s32) exp - cast(s32) (infoF64.mantbits) - 1)

	// d = mant << (exp - mantbits)
	// Next lowest floating point number is mant-1 << exp-mantbits,
	// unless mant-1 drops the significant bit and exp is not the minimum exp,
	// in which case the next lowest is mant*2-1 << exp-mantbits-1.
	// Either way, call it mantlo << explo-mantbits.
	// Our lower bound is halfway between, mantlo*2+1 << explo-mantbits-1.
	var mantlo: u64
	var explo: s64
	if mant > (1'u64 << infoF64.mantbits) || exp == minexp
    {
		mantlo = mant - 1
		explo = exp
	} else
    {
		mantlo = mant*2 - 1
		explo = exp - 1
	}

	var lower: Decimal
	lower.assign(mantlo*2 + 1)
	lower.shift(cast(s32) explo - cast(s32)(infoF64.mantbits) - 1)

	// The upper and lower bounds are possible outputs only if
	// the original mantissa is even, so that IEEE round-to-even
	// would round to the original mantissa and not the neighbors.
	inclusive := mant % 2 == 0

	// As we walk the digits we want to know whether rounding up would fall
	// within the upper bound. This is tracked by upperdelta:
	//
	// If upperdelta == 0, the digits of d and upper are the same so far.
	//
	// If upperdelta == 1, we saw a difference of 1 between d and upper on a
	// previous digit and subsequently only 9s for d and 0s for upper.
	// (Thus rounding up may fall outside the bound, if it is exclusive.)
	//
	// If upperdelta == 2, then the difference is greater than 1
	// and we know that rounding up falls within the bound.
	var upperdelta: u8

	// Now we can figure out the minimum number of digits required.
	// Walk along until d has distinguished itself from upper and lower.
    ui := 0
    while true
	{
		// lower, d, and upper may have the decimal points at different
		// places. In this case upper is the longest, so we iterate from
		// ui==0 and start li and mi at (possibly) -1.
		mi := ui - upper.dp + d.dp
		if mi >= d.nd
			break
		li := ui - upper.dp + lower.dp
		l := "0"'u8 // lower digit
		if li >= 0 && li < lower.nd
			l = lower.d[li]
		m := "0"'u8 // middle digit
		if mi >= 0
			m = d.d[mi]
		u := "0"'u8 // upper digit
		if ui < upper.nd
			u = upper.d[ui]

		// Okay to round down (truncate) if lower has a different digit
		// or if lower is inclusive and is exactly the result of rounding
		// down (i.e., and we have reached the final digit of lower).
		okdown := l != m || inclusive && li+1 == lower.nd

		switch
        {
		case upperdelta == 0 && m+1 < u:
			// Example:
			// m = 12345xxx
			// u = 12347xxx
			upperdelta = 2
		case upperdelta == 0 && m != u:
			// Example:
			// m = 12345xxx
			// u = 12346xxx
			upperdelta = 1
		case upperdelta == 1 && (m != "9"'u8 || u != "0"'u8):
			// Example:
			// m = 1234598x
			// u = 1234600x
			upperdelta = 2
		}

		// Okay to round up if upper has a different digit and either upper
		// is inclusive or upper is bigger than the result of rounding up.
		okup := upperdelta > 0 && (inclusive || upperdelta > 1 || ui+1 < upper.nd)

		// If it's okay to do either, then round to the nearest one.
		// If it's okay to do only one, do it.
		switch
        {
		case okdown && okup:
			d.round(mi + 1)
			return
		case okdown:
			d.roundDown(mi + 1)
			return
		case okup:
			d.roundUp(mi + 1)
			return
		}

        ui += 1
	}
}