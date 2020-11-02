public namespace std.unicode16;

// Convert an utf8 buffer to a 16 bits unicode sequence, and returns the
// number of valid elements in the destination buffer
func fromUtf8(dest: [..] u16, src: const [..] u8)->u32
{
    count := 0'u32
    srcPtr := @dataof(src)
    destPtr := @dataof(dest)
    srcLength := @countof(src)

    (c, eat) := utf8.firstChar(src)
    while eat != 0
    {
        debug.assert(count != @countof(dest), "destination buffer is too small")
        debug.assert(c <= acast swag.MaxU16, "utf8 character overflow")

        :destPtr = cast(u16) c
        destPtr, count += 1

        srcPtr += eat
        srcLength -= eat
        (c, eat) = utf8.firstChar(@mkslice(srcPtr, srcLength))
    }

    return count
}

// Convert an utf8 buffer to a 16 bits unicode sequence
func fromUtf8(src: const [..] u8)->Array'u16
{
    var result: Array'u16
    result.reserve(@countof(src))
    result.count = fromUtf8(@mkslice(result.buffer, @countof(src)), src)
    return result
}