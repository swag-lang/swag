// https://github.com/golang/go/blob/11f159456b1dba3ec499da916852dd188d1e04a7/src/strconv/ftoa.go

#global namespace strconv
#global private
#global #[swag.safety("of", false)]
using globalization

func dtoa(dst: *ConcatBuffer, precision: s64, fmt: u8, neg, forceSign: bool, mant: u64, exp: s64)
{
    prec := precision

	var d: Decimal
	d.assign(mant)
	d.shift(cast(s32) exp - swag.F64.MantBits)

	var digs: DecimalSlice
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

	formatDigits(dst, shortest, neg, forceSign, digs, prec, fmt)
}

func formatDigits(dst: *ConcatBuffer, shortest: bool, neg, forceSign: bool, digs: DecimalSlice, precision: s64, fmt: u8)
{
    prec := precision
	switch fmt
    {
	case "e"'u8, "E"'u8:
		return fmtE(dst, neg, forceSign, digs, prec, fmt)

	case "f"'u8:
		return fmtF(dst, neg, forceSign, digs, prec)

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
			return fmtE(dst, neg, forceSign, digs, prec - 1, cast(u8) (fmt + "e"'u8 - "g"'u8))
		}

		if prec > digs.dp
            prec = digs.nd
		return fmtF(dst, neg, forceSign, digs, math.max(prec - digs.dp, 0))
	}
}

func fmtE(dst: *ConcatBuffer, neg, forceSign: bool, d: DecimalSlice, prec: s64, fmt: u8)
{
	// sign
	if neg
		dst.addByte(g_CultureInfo.numberFormat.negativeSign)
    else if forceSign
        dst.addByte(g_CultureInfo.numberFormat.positiveSign)

	// first digit
	ch := "0"'u8
	if d.nd != 0
		ch = d.d[0]
	dst.addByte(ch)

	// .moredigits
	if prec > 0
    {
		dst.addByte(g_CultureInfo.numberFormat.decimalSeparator)
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

func fmtF(dst: *ConcatBuffer, neg, forceSign: bool, d: DecimalSlice, prec: s64)
{
	// sign
	if neg
		dst.addByte(g_CultureInfo.numberFormat.negativeSign)
    else if forceSign
        dst.addByte(g_CultureInfo.numberFormat.positiveSign)

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
		dst.addByte(g_CultureInfo.numberFormat.decimalSeparator)
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
	minexp := swag.F64.Bias + 1 // minimum possible exponent
	if exp > minexp && 332*(d.dp-d.nd) >= 100*(exp-cast(s64)(swag.F64.MantBits))
    {
		// The number is already shortest.
		return
	}

	// d = mant << (exp - mantbits)
	// Next highest floating point number is mant+1 << exp-mantbits.
	// Our upper bound is halfway between, mant*2+1 << exp-mantbits-1.
	var upper: Decimal
	upper.assign(mant*2 + 1)
	upper.shift(cast(s32) exp - cast(s32) (swag.F64.MantBits) - 1)

	// d = mant << (exp - mantbits)
	// Next lowest floating point number is mant-1 << exp-mantbits,
	// unless mant-1 drops the significant bit and exp is not the minimum exp,
	// in which case the next lowest is mant*2-1 << exp-mantbits-1.
	// Either way, call it mantlo << explo-mantbits.
	// Our lower bound is halfway between, mantlo*2+1 << explo-mantbits-1.
	var mantlo: u64
	var explo: s64
	if mant > (1'u64 << swag.F64.MantBits) || exp == minexp
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
	lower.shift(cast(s32) explo - cast(s32)(swag.F64.MantBits) - 1)

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

// The format 'fmt' is one of
// 'e' (-d.dddde±dd, a decimal exponent)
// 'E' (-d.ddddE±dd, a decimal exponent)
// 'f' (-ddd.dddd, no exponent)
// 'g' ('e' for large exponents, 'f' otherwise)
// 'G' ('E' for large exponents, 'f' otherwise)
public func dtoa(buf: *ConcatBuffer, val: f64, fmt: u8, prec: s64, forceSign: bool)
{
	var bits: u64 = bitcast(u64) val

	neg  := bits >> (swag.F64.ExpBits + swag.F64.MantBits) != 0
	exp  := cast(s64) (bits >> swag.F64.MantBits) & ((1 << swag.F64.ExpBits) - 1)
	mant := bits & ((1'u64 << swag.F64.MantBits) - 1)

	switch exp
    {
    // Inf, NaN
	case (1 << swag.F64.ExpBits) - 1:
		var s: string
		switch
        {
		case mant != 0: s = "nan"
		case neg:       s = "-inf"
		default:        s = "+inf"
		}

		buf.addBytes(s)
        return

	case 0:
		exp += 1

	default:
		mant |= 1'u64 << swag.F64.MantBits
	}

	exp += swag.F64.Bias

/*	var digs DecimalSlice
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
			return dtoa(dst, prec, fmt, neg, mant, exp, flt)
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
		return dtoa(dst, prec, fmt, neg, mant, exp, flt)
	}
	return formatDigits(dst, shortest, neg, digs, prec, fmt)*/
    dtoa(buf, prec, fmt, neg, forceSign, mant, exp)
}
