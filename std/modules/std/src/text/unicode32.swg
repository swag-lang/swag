namespace std.unicode32;

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
}

// Convert an utf8 buffer to a character sequence, and returns the
// number of valid elements in the destination buffer
public func fromUtf8(dest: [..] char, src: const [..] u8)->u32
{
    count := 0'u32
    srcPtr := @dataof(src)
    destPtr := @dataof(dest)
    srcLength := @countof(src)

    (c, eat) := utf8.firstChar(src)
    while eat != 0
    {
        debug.assert(count != @countof(dest), "destination buffer is too small")

        :destPtr = c
        destPtr, count += 1

        srcPtr += eat
        srcLength -= eat
        (c, eat) = utf8.firstChar(@mkslice(srcPtr, srcLength))
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

////////// TABLE ////////

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
    lo := 0'u32
    hi := @countof(ranges)
    while lo < hi
    {
        m := lo + (hi-lo)/2
        range := &ranges[m]
        if range.lo <= r <= range.hi
            return range.stride == 1 || (r-range.lo) % range.stride == 0
        if r < range.lo
            hi = m
        else
            lo = m + 1
    }

    return false
}

struct Range
{
    lo, hi: char
    stride: u32
}

struct CaseRangeDelta
{
    x, y, z: s32
}

struct CaseRange
{
    lo, hi: u32
    d:      CaseRangeDelta
}

const UpperLower = swag.MaxS32