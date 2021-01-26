#global namespace unicode32

#[swag.inline, swag.constexpr]
public
{
    func isLatin1(c: char)  => c <= 0xFF
    func isAscii(c: char)   => c <= 0x7F
}

#[swag.constexpr]
public
{
    func isDigit(c: char)->bool
    {
        if isLatin1(c) return latin1.isDigit(cast(u8) c)
        return isInTable(TableDigit, c)
    }

    func isNumber(c: char)->bool
    {
        if isLatin1(c) return latin1.isNumber(cast(u8) c)
        return isInTable(TableNumber, c)
    }

    func isControl(c: char)->bool
    {
        if isLatin1(c) return latin1.isControl(cast(u8) c)
        return false
    }

    func isSpace(c: char)->bool
    {
        if isLatin1(c) return latin1.isSpace(cast(u8) c)
        return isInTable(TableWhiteSpace, c)
    }

    func isLetter(c: char)->bool
    {
        if isLatin1(c) return latin1.isLetter(cast(u8) c)
        return isInTable(TableLetter, c)
    }

    func isSymbol(c: char)->bool
    {
        if isLatin1(c) return latin1.isSymbol(cast(u8) c)
        return isInTable(TableSymbol, c)
    }

    func isLetterOrDigit(c: char)->bool
    {
        if isLatin1(c) return latin1.isLetterOrDigit(cast(u8) c)
        return isDigit(c) || isLetter(c)
    }

    func isTitle(c: char)->bool
    {
        if isLatin1(c) return false
        return isInTable(TableTitle, c)
    }

    func isLower(c: char)->bool
    {
        if isLatin1(c) return latin1.isLower(cast(u8) c)
        return isInTable(TableLower, c)
    }

    func isUpper(c: char)->bool
    {
        if isLatin1(c) return latin1.isUpper(cast(u8) c)
        return isInTable(TableUpper, c)
    }

    func toLower(c: char)->char
    {
        if isLatin1(c) return autocast latin1.toLower(cast(u8) c)

        res := caseChangeTo(WantedCase.LowerCase, c, TableSpecialCaseRanges)
        if res.foundMapping return res.mappedRune
        res = caseChangeTo(WantedCase.LowerCase, c, TableCaseRanges)
        if !res.foundMapping return c
        return res.mappedRune
    }

    func toUpper(c: char)->char
    {
        if isLatin1(c) return autocast latin1.toUpper(cast(u8) c)

        res := caseChangeTo(WantedCase.UpperCase, c, TableSpecialCaseRanges)
        if res.foundMapping return res.mappedRune
        res = caseChangeTo(WantedCase.UpperCase, c, TableCaseRanges)
        if !res.foundMapping return c

        return res.mappedRune
    }

    func toTitle(c: char)->char
    {
        if isLatin1(c) return autocast latin1.toUpper(cast(u8) c)

        res := caseChangeTo(WantedCase.TitleCase, c, TableSpecialCaseRanges)
        if res.foundMapping return res.mappedRune
        res = caseChangeTo(WantedCase.TitleCase, c, TableCaseRanges)
        if !res.foundMapping return c

        return res.mappedRune
    }
}

// Convert an utf8 buffer to a character sequence, and returns the
// number of valid elements in the destination buffer
public func fromUtf8(dest: [..] char, src: const [..] u8)->u32
{
    count := 0'u32
    srcPtr := @dataof(src)
    destPtr := @dataof(dest)
    srcLength := @countof(src)

    (c, eat) := utf8.decodeChar(src)
    while eat != 0
    {
        debug.assert(count != @countof(dest), "destination buffer is too small")

        :destPtr = c
        destPtr, count += 1

        srcPtr += eat
        srcLength -= eat
        (c, eat) = utf8.decodeChar(@mkslice(srcPtr, srcLength))
    }

    return count
}

// Convert an utf8 buffer to a character sequence
public func fromUtf8(src: const [..] u8)->Array'char
{
    var result: retval
    result.reserve(@countof(src))
    result.count = fromUtf8(@mkslice(result.buffer, @countof(src)), src)
    return result
}

// Make a char buffer upper case
public func makeUpper(buffer: [..] char)
{
    p := @dataof(buffer)
    loop @countof(buffer)
    {
        :p = toUpper(:p)
        p += 1
    }
}

// Make a char buffer lower case
public func makeLower(buffer: [..] char)
{
    p := @dataof(buffer)
    loop @countof(buffer)
    {
        :p = toLower(:p)
        p += 1
    }
}

// Compare two unicode strings, and returns -1, 0 or 1
func compare(src, dst: const [..] char, ignoreCase = false)->s32
{
    length := @countof(src)
    otherLength := @countof(dst)
    minLen := math.min(length, otherLength)

    var result: s32
    if ignoreCase
    {
        #[swag.safety("bc", false)]
        loop i: minLen
        {
            c1 := toLower(src[i])
            c2 := toLower(dst[i])
            result = cast(s32) c1 - cast(s32) c2
            if result != 0 return math.sign(result)
        }
    }
    else
    {
        result = memory.compare(@dataof(src), @dataof(dst), minLen)
    }

    if result != 0 return math.sign(result)
    return length <=> otherLength
}

////////// TABLES ////////
const UpperLower = swag.MaxS32

struct Range
{
    lo, hi: char
    stride: u32
}

struct CaseRange
{
    lo, hi: char
    delta:  [3] s32
}

#[swag.enumindex]
enum WantedCase
{
	UpperCase
	LowerCase
	TitleCase
}

// https://golang.org/src/unicode/letter.go
func isInTable(ranges: const [..] Range, r: char)->bool
{
    // linear search for a small array or a latin1 character
    const LinearLimit = 18
    if @countof(ranges) <= LinearLimit || r <= 255
    {
        visit *range: ranges
        {
            if r < range.lo  return false
            if r <= range.hi return range.stride == 1 || (r - range.lo) % range.stride == 0
        }

        return false
    }

    // binary search over ranges
    lo := 0'uint
    hi := @countof(ranges)
    while lo < hi
    {
        m := lo + (hi-lo)/2
        range := &ranges[m]
        if range.lo <= r && r <= range.hi
            return range.stride == 1 || (r-range.lo) % range.stride == 0
        if r < range.lo
            hi = m
        else
            lo = m + 1
    }

    return false
}

// https://golang.org/src/unicode/letter.go
func caseChangeTo(wCase: WantedCase, r: char, caseRange: const [..] CaseRange) -> {mappedRune: char, foundMapping: bool}
{
    var result: retval

	// binary search over ranges
	lo := 0'uint
	hi := @countof(caseRange)
	while lo < hi
    {
		m := lo + (hi-lo) / 2
		cr := caseRange[m]
		if cr.lo <= r && r <= cr.hi
        {
			delta := cr.delta[wCase]
			if delta == swag.MaxS32
            {
				// In an Upper-Lower sequence, which always starts with
				// an UpperCase letter, the real deltas always look like:
				//	{0, 1, 0}    UpperCase (Lower is next)
				//	{-1, 0, -1}  LowerCase (Upper, Title are previous)
				// The characters at even offsets from the beginning of the
				// sequence are upper case; the ones at odd offsets are lower.
				// The correct mapping can be done by clearing or setting the low
				// bit in the sequence offset.
				// The constants UpperCase and TitleCase are even while LowerCase
				// is odd so we take the low bit from _case.
				result.mappedRune = cr.lo + ((r - cr.lo) & ~1 | (cast(char) wCase & 1))
                result.foundMapping = true
                return result
			}

            #[swag.safety("of", false)]
            result.mappedRune = r + cast(char) delta
            result.foundMapping = true
            return result
		}

		if r < cr.lo
			hi = m
		else
			lo = m + 1
	}

	return result
}