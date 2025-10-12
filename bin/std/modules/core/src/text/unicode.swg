/*
    :LICENCE
    This code is based on the Go library (https://golang.org/src/unicode/letter.go)
    The original code has been modified for Swag
    See LICENCE.md for the corresponding licence.
*/

#global namespace Unicode

#[Swag.Inline, Swag.ConstExpr]
public
{
    func isLatin1(c: rune) => c <= 0xFF
    func isAscii(c: rune)  => c <= 0x7F
}

#[Swag.ConstExpr]
public
{
    func isDigit(c: rune)->bool
    {
        if isLatin1(c) do
            return Latin1.isDigit(cast(u8) c)
        return isInTable(TableDigit, c)
    }

    func isNumber(c: rune)->bool
    {
        if isLatin1(c) do
            return Latin1.isNumber(cast(u8) c)
        return isInTable(TableNumber, c)
    }

    func isControl(c: rune)->bool
    {
        if isLatin1(c) do
            return Latin1.isControl(cast(u8) c)
        return false
    }

    func isSpace(c: rune)->bool
    {
        if isLatin1(c) do
            return Latin1.isSpace(cast(u8) c)
        return isInTable(TableWhiteSpace, c)
    }

    func isLetter(c: rune)->bool
    {
        if isLatin1(c) do
            return Latin1.isLetter(cast(u8) c)
        return isInTable(TableLetter, c)
    }

    func isSymbol(c: rune)->bool
    {
        if isLatin1(c) do
            return Latin1.isSymbol(cast #bit (u8) c)
        return isInTable(TableSymbol, c)
    }

    func isSymbolMath(c: rune)->bool
    {
        return isInTable(TableSymbolMath, c)
    }

    func isLetterOrDigit(c: rune)->bool
    {
        if isLatin1(c) do
            return Latin1.isLetterOrDigit(cast(u8) c)
        return isDigit(c) or isLetter(c)
    }

    func isTitle(c: rune)->bool
    {
        if isLatin1(c) do
            return false
        return isInTable(TableTitle, c)
    }

    func isLower(c: rune)->bool
    {
        if isLatin1(c) do
            return Latin1.isLower(cast(u8) c)
        return isInTable(TableLower, c)
    }

    func isUpper(c: rune)->bool
    {
        if isLatin1(c) do
            return Latin1.isUpper(cast(u8) c)
        return isInTable(TableUpper, c)
    }

    func isWord(c: rune)->bool
    {
        return isLetter(c) or isDigit(c) or c == '_'
    }

    func toLower(c: rune)->rune
    {
        if isLatin1(c) do
            return cast() Latin1.toLower(cast(u8) c)

        var res = caseChangeTo(WantedCase.LowerCase, c, TableSpecialCaseRanges)
        if res.foundMapping do
            return res.mappedRune
        res = caseChangeTo(WantedCase.LowerCase, c, TableCaseRanges)
        if !res.foundMapping do
            return c
        return res.mappedRune
    }

    func toUpper(c: rune)->rune
    {
        if isLatin1(c) do
            return cast() Latin1.toUpper(cast(u8) c)

        var res = caseChangeTo(WantedCase.UpperCase, c, TableSpecialCaseRanges)
        if res.foundMapping do
            return res.mappedRune
        res = caseChangeTo(WantedCase.UpperCase, c, TableCaseRanges)
        if !res.foundMapping do
            return c

        return res.mappedRune
    }

    func toTitle(c: rune)->rune
    {
        if isLatin1(c) do
            return cast() Latin1.toUpper(cast(u8) c)

        var res = caseChangeTo(WantedCase.TitleCase, c, TableSpecialCaseRanges)
        if res.foundMapping do
            return res.mappedRune
        res = caseChangeTo(WantedCase.TitleCase, c, TableCaseRanges)
        if !res.foundMapping do
            return c

        return res.mappedRune
    }
}

// Convert an utf8 buffer to a character sequence, and returns the
// number of valid elements in the destination buffer
#[Swag.Overload]
public func fromUtf8(dest: [..] rune, src: const [..] u8)->u32
{
    var count     = 0'u32
    var srcPtr    = @dataof(src)
    var destPtr   = @dataof(dest)
    var srcLength = @countof(src)

    var (c, eat) = Utf8.decodeRune(src)
    while eat != 0
    {
        Debug.assert(count != @countof(dest), "destination buffer is too small")

        dref destPtr = c
        destPtr, count += 1

        srcPtr += eat
        srcLength -= eat
        (c, eat) = Utf8.decodeRune(@mkslice(srcPtr, srcLength))
    }

    return count
}

// Convert an utf8 buffer to a character sequence
#[Swag.Overload]
public func fromUtf8(src: const [..] u8)->Array'rune
{
    var result: retval
    result.reserve(@countof(src))
    result.count = fromUtf8(@mkslice(result.buffer, @countof(src)), src)
    return result
}

// Make a rune buffer upper case
public func makeUpper(buffer: [..] rune)
{
    var p = @dataof(buffer)
    for @countof(buffer)
    {
        dref p = toUpper(dref p)
        p += 1
    }
}

// Make a rune buffer lower case
public func makeLower(buffer: [..] rune)
{
    var p = @dataof(buffer)
    for @countof(buffer)
    {
        dref p = toLower(dref p)
        p += 1
    }
}

// Compare two unicode strings, and returns -1, 0 or 1
func compare(src, dst: const [..] rune, ignoreCase = false, len: u64 = 0)->s32
{
    let length      = @countof(src)
    let otherLength = @countof(dst)

    var minLen: u64 = undefined
    if len
    {
        if length < len do
            return -1
        if otherLength < len do
            return 1
        minLen = len
    }
    else
    {
        minLen = Math.min(length, otherLength)
    }

    var result: s32
    if ignoreCase
    {
        #[Swag.Safety(.All, .BoundCheck, false)]
        for i in minLen
        {
            let c1 = toLower(src[i])
            let c2 = toLower(dst[i])
            result = cast(s32) c1 - cast(s32) c2
            if result != 0 do
                return Math.sign(result)
        }
    }
    else
    {
        result = Memory.compare(@dataof(src), @dataof(dst), minLen * #sizeof(rune))
    }

    if result != 0 do
        return Math.sign(result)
    if len do
        return 0
    return length <=> otherLength
}

////////// TABLES ////////
const UpperLower = Swag.S32.Max

struct Range
{
    lo, hi:     rune
    stride:     u32
}

struct CaseRange
{
    lo, hi:     rune
    delta:      [3] s32
}

#[Swag.EnumIndex]
enum WantedCase
{
    UpperCase
    LowerCase
    TitleCase
}

func isInTable(ranges: const [..] Range, r: rune)->bool
{
    // linear search for a small array or a latin1 character
    const LinearLimit = 18
    if @countof(ranges) <= LinearLimit or r <= 255
    {
        foreach &range in ranges
        {
            if r < range.lo do
                return false
            if r <= range.hi do
                return range.stride == 1 or (r - range.lo) % range.stride == 0
        }

        return false
    }

    // binary search over ranges
    var lo = 0'u64
    var hi = @countof(ranges)
    while lo < hi
    {
        let m     = lo + (hi - lo) / 2
        let range = &ranges[m]
        if range.lo <= r and r <= range.hi
        {
            return range.stride == 1 or (r - range.lo) % range.stride == 0
        }

        if r < range.lo do
            hi = m
        else do
            lo = m + 1
    }

    return false
}

func caseChangeTo(wCase: WantedCase, r: rune, caseRange: const [..] CaseRange)->{ mappedRune: rune, foundMapping: bool }
{
    var result: retval

    // binary search over ranges
    var lo = 0'u64
    var hi = @countof(caseRange)
    while lo < hi
    {
        let m  = lo + (hi - lo) / 2
        let cr = caseRange[m]
        if cr.lo <= r and r <= cr.hi
        {
            let delta = cr.delta[wCase]
            if delta == Swag.S32.Max
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
                result.mappedRune   = cr.lo + #wrap ((r - cr.lo) & ~1'u32 | (cast(rune) wCase & 1))
                result.foundMapping = true
                return result
            }

            result.mappedRune   = r + #wrap cast #bit (rune) delta
            result.foundMapping = true
            return result
        }

        if r < cr.lo do
            hi = m
        else do
            lo = m + 1
    }

    return result
}
