/*
    :LICENCE
    This code is based on the Go library (https://github.com/golang/go/blob/master/src/strconv/ftoa.go)
    The original code has been modified for Swag
    See LICENCE.md for the corresponding licence.
*/

#global namespace StrConv
#global namespace Dtoa
#global #[Swag.Safety(.All, .Overflow|.NaN, false)]
using Globalization

func dtoa(dst: *ConcatBuffer, precision: s64, fmt: u8, neg, forceSign: bool, mant: u64, exp: s64)
{
    var prec = precision

    var d: Decimal
    d.assign(mant)
    d.shift(cast(s32) exp - Swag.F64.MantBits)

    var digs:    DecimalSlice
    let shortest = prec < 0
    if shortest
    {
        roundShortest(&d, mant, exp)
        digs.d  = d.d
        digs.nd = d.nd
        digs.dp = d.dp
        // Precision for shortest representation mode.
        switch fmt
        {
        case 'e''u8, 'E''u8:
            prec = digs.nd - 1
        case 'f''u8:
            prec = Math.max(digs.nd - digs.dp, 0)
        case 'g''u8, 'G''u8:
            prec = digs.nd
        }
    }
    else
    {
        // Round appropriately.
        switch fmt
        {
        case 'e''u8, 'E''u8:
            d.round(prec + 1)
        case 'f''u8:
            d.round(d.dp + prec)
        case 'g''u8, 'G''u8:
            d.round(prec == 0 ? 1 : prec)
        }

        digs.d  = d.d
        digs.nd = d.nd
        digs.dp = d.dp
    }

    formatDigits(dst, shortest, neg, forceSign, digs, prec, fmt)
}

func formatDigits(dst: *ConcatBuffer, shortest: bool, neg, forceSign: bool, digs: DecimalSlice, precision: s64, fmt: u8)
{
    var prec = precision
    switch fmt
    {
    case 'e''u8, 'E''u8:
        return fmtE(dst, neg, forceSign, digs, prec, fmt)

    case 'f''u8:
        return fmtF(dst, neg, forceSign, digs, prec)

    case 'g''u8, 'G''u8:
        // trailing fractional zeros in 'e' form will be trimmed.
        var eprec = prec
        if eprec > digs.nd and digs.nd >= digs.dp do
            eprec = digs.nd
        // %e is used if the exponent from the conversion
        // is less than -4 or greater than or equal to the precision.
        // if precision was the shortest possible, use precision 6 for this decision.
        if shortest do
            eprec = 6
        let exp = digs.dp - 1
        if exp < -4 or exp >= eprec
        {
            if prec > digs.nd do
                prec = digs.nd
            return fmtE(dst, neg, forceSign, digs, prec - 1, cast(u8) (fmt + 'e''u8 - 'g''u8))
        }

        if prec > digs.dp do
            prec = digs.nd
        return fmtF(dst, neg, forceSign, digs, Math.max(prec - digs.dp, 0))
    }
}

func fmtE(dst: *ConcatBuffer, neg, forceSign: bool, d: DecimalSlice, prec: s64, fmt: u8)
{
    // sign
    if neg do
        dst.addNative(g_CultureInfo.numberFormat.negativeSign)
    elif forceSign do
        dst.addNative(g_CultureInfo.numberFormat.positiveSign)

    // first digit
    var ch = '0''u8
    if d.nd != 0 do
        ch = d.d[0]
    dst.addNative(ch)

    // .moredigits
    if prec > 0
    {
        dst.addNative(g_CultureInfo.numberFormat.decimalSeparator)
        var i = 1's64
        let m = Math.min(d.nd, prec + 1)
        if i < m
        {
            dst.addBytes(@mkslice(&d.d[i], m - i))
            i = m
        }

        while i <= prec
        {
            dst.addBytes("0")
            i += 1
        }
    }

    // e±
    dst.addNative(fmt)
    var exp = d.dp - 1
    if d.nd == 0 do
        exp = 0
    if exp < 0
    {
        ch  = '-''u8
        exp = -exp
    }
    else
    {
        ch = '+''u8
    }

    dst.addNative(ch)

    // dd or ddd
    switch
    {
    case exp < 10:
        dst.addNative('0''u8)
        dst.addNative(cast(u8) (exp + '0''u8))
    case exp < 100:
        dst.addNative(cast(u8) ((exp / 10) + '0''u8))
        dst.addNative(cast(u8) ((exp % 10) + '0''u8))
    default:
        dst.addNative(cast(u8) ((exp / 100) + '0''u8))
        dst.addNative(cast(u8) (((exp / 10) % 10) + '0''u8))
        dst.addNative(cast(u8) ((exp % 10) + '0''u8))
    }
}

func fmtF(dst: *ConcatBuffer, neg, forceSign: bool, d: DecimalSlice, prec: s64)
{
    // sign
    if neg do
        dst.addNative(g_CultureInfo.numberFormat.negativeSign)
    elif forceSign do
        dst.addNative(g_CultureInfo.numberFormat.positiveSign)

    // integer, padded with zeros as needed.
    if d.dp > 0
    {
        var m = Math.min(d.nd, d.dp)
        dst.addBytes(@mkslice(&d.d[0], m))
        while m < d.dp
        {
            dst.addNative('0''u8)
            m += 1
        }
    }
    else do
        dst.addNative('0''u8)

    // fraction
    if prec > 0
    {
        dst.addNative(g_CultureInfo.numberFormat.decimalSeparator)
        for var i = 0; i < prec; i += 1
        {
            var ch = '0''u8
            let j  = d.dp + i
            if 0 <= j and j < d.nd do
                ch = d.d[j]
            dst.addNative(ch)
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
    let minexp = Swag.F64.Bias + 1 // minimum possible exponent
    if exp > minexp and 332 * (d.dp - d.nd) >= 100 * (exp - cast(s64) Swag.F64.MantBits)
    {
        // The number is already shortest.
        return
    }

    // d = mant << (exp - mantbits)
    // Next highest floating point number is mant+1 << exp-mantbits.
    // Our upper bound is halfway between, mant*2+1 << exp-mantbits-1.
    var upper: Decimal
    upper.assign(mant * 2 + 1)
    upper.shift(cast(s32) exp - cast(s32) Swag.F64.MantBits - 1)

    // d = mant << (exp - mantbits)
    // Next lowest floating point number is mant-1 << exp-mantbits,
    // unless mant-1 drops the significant bit and exp is not the minimum exp,
    // in which case the next lowest is mant*2-1 << exp-mantbits-1.
    // Either way, call it mantlo << explo-mantbits.
    // Our lower bound is halfway between, mantlo*2+1 << explo-mantbits-1.
    var mantlo: u64
    var explo:  s64
    if mant > (1'u64 << Swag.F64.MantBits) or exp == minexp
    {
        mantlo = mant - 1
        explo  = exp
    }
    else
    {
        mantlo = mant * 2 - 1
        explo  = exp - 1
    }

    var lower: Decimal
    lower.assign(mantlo * 2 + 1)
    lower.shift(cast(s32) explo - cast(s32) Swag.F64.MantBits - 1)

    // The upper and lower bounds are possible outputs only if
    // the original mantissa is even, so that IEEE round-to-even
    // would round to the original mantissa and not the neighbors.
    let inclusive = mant % 2 == 0

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
    var ui = 0
    for
    {
        // lower, d, and upper may have the decimal points at different
        // places. In this case upper is the longest, so we iterate from
        // ui==0 and start li and mi at (possibly) -1.
        let mi = ui - upper.dp + d.dp
        if mi >= d.nd do
            break
        let li = ui - upper.dp + lower.dp
        var l  = '0''u8                       // lower digit
        if li >= 0 and li < lower.nd do
            l = lower.d[li]
        var m = '0''u8 // middle digit
        if mi >= 0 do
            m = d.d[mi]
        var u = '0''u8 // upper digit
        if ui < upper.nd do
            u = upper.d[ui]

        // Okay to round down (truncate) if lower has a different digit
        // or if lower is inclusive and is exactly the result of rounding
        // down (i.e., and we have reached the final digit of lower).
        let okdown = l != m or inclusive and li + 1 == lower.nd

        switch
        {
        case upperdelta == 0 and m + 1 < u:
            // Example:
            // m = 12345xxx
            // u = 12347xxx
            upperdelta = 2
        case upperdelta == 0 and m != u:
            // Example:
            // m = 12345xxx
            // u = 12346xxx
            upperdelta = 1
        case upperdelta == 1 and (m != '9''u8 or u != '0''u8):
            // Example:
            // m = 1234598x
            // u = 1234600x
            upperdelta = 2
        }

        // Okay to round up if upper has a different digit and either upper
        // is inclusive or upper is bigger than the result of rounding up.
        let okup = upperdelta > 0 and (inclusive or upperdelta > 1 or ui + 1 < upper.nd)

        // If it's okay to: either, then round to the nearest one.
        // If it's okay to: only one,: it.
        switch
        {
        case okdown and okup:
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

// Convert of floating value to a string.
// The format 'fmt' is one of:
// | e | -d.dddde±dd, a decimal exponent
// | E | -d.ddddE±dd, a decimal exponent
// | f | -ddd.dddd, no exponent
// | g | 'e' for large exponents, 'f' otherwise
// | G | 'E' for large exponents, 'f' otherwise
// A negative precision means *only as much as needed to be exact*
public func parse(buf: *ConcatBuffer, val: f64, fmt: u8, precision: s64, forceSign: bool)
{
    let bits: u64 = cast #bit (u64) val

    var prec = precision
    let neg  = bits >> (Swag.F64.ExpBits + Swag.F64.MantBits) != 0
    var exp  = cast(s64) (bits >> Swag.F64.MantBits) & ((1 << Swag.F64.ExpBits) - 1)
    var mant = bits & ((1'u64 << Swag.F64.MantBits) - 1)

    switch exp
    {
    case (1 << Swag.F64.ExpBits) - 1:
        var s: string
        switch
        {
        case mant != 0:
            s = "NaN"
        case neg:
            s = "-Inf"
        default:
            s = "+Inf"
        }

        buf.addBytes(s)
        return

    case 0:
        exp += 1

    default:
        mant |= 1'u64 << Swag.F64.MantBits
    }

    exp += Swag.F64.Bias

    var digs: DecimalSlice

    var ok       = false
    let shortest = prec < 0
    if shortest
    {
        // Try Grisu3 algorithm.
        var f: FloatExt
        var (lower, upper) = f.assignComputeBounds(mant, exp, neg)
        var tmp: [32] u8
        digs.d = tmp

        ok = f.shortestDecimal(&digs, &lower, &upper)
        if ok
        {
            // Precision for shortest representation mode.
            switch fmt
            {
            case 'e''u8, 'E''u8:
                prec = Math.max(digs.nd - 1, 0)
            case 'f''u8:
                prec = Math.max(digs.nd - digs.dp, 0)
            case 'g''u8, 'G''u8:
                prec = digs.nd
            }

            return formatDigits(buf, shortest, neg, forceSign, digs, prec, fmt)
        }
    }
    elif fmt != 'f''u8
    {
        // Fixed number of digits.
        var digits = prec
        switch fmt
        {
        case 'e''u8, 'E''u8:
            digits += 1
        case 'g''u8, 'G''u8:
            if prec == 0 do
                prec = 1
            digits = prec
        }

        // try fast algorithm when the number of digits is reasonable.
        if digits <= 15
        {
            var tmp: [24] u8
            digs.d = tmp
            var f = FloatExt{mant, exp - cast(s64) Swag.F64.MantBits, neg}
            ok = f.fixedDecimal(&digs, digits)
            if ok do
                return formatDigits(buf, shortest, neg, forceSign, digs, prec, fmt)
        }
    }

    return dtoa(buf, prec, fmt, neg, forceSign, mant, exp)
}
