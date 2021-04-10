#global namespace strconv
#global private
#global #[swag.safety("of", false)]

struct FloatInfo
{
	mantbits: u32
	expbits:  u32
	bias:     s32
}

const infoF64: FloatInfo = @{52, 11, -1023}

// The format fmt is one of
// 'b' (-ddddp±ddd, a binary exponent),
// 'e' (-d.dddde±dd, a decimal exponent),
// 'E' (-d.ddddE±dd, a decimal exponent),
// 'f' (-ddd.dddd, no exponent),
// 'g' ('e' for large exponents, 'f' otherwise),
// 'G' ('E' for large exponents, 'f' otherwise),
// 'x' (-0xd.ddddp±ddd, a hexadecimal fraction and binary exponent), or
// 'X' (-0Xd.ddddP±ddd, a hexadecimal fraction and binary exponent).

func f64ToA(buf: *ConcatBuffer, val: f64, fmt: u8, prec: s32)
{
	var bits: u64 = bitcast(u64) val

	neg  := bits >> (infoF64.expbits + infoF64.mantbits) != 0
	exp  := cast(s32) (bits >> infoF64.mantbits) & ((1 << infoF64.expbits) - 1)
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
}