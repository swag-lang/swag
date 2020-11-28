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
        return false
    }

    func isLetterOrDigit(c: char)->bool
    {
        if isLatin1(c) return latin1.isLetterOrDigit(cast(u8) c)
        return false
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