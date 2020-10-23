public namespace std.unicode16;

// Convert an utf8 buffer to a 16 bits unicode sequence
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
        destPtr[0] = cast(u16) c
        destPtr, count += 1
        srcLength -= eat
        (c, eat) = utf8.firstChar(@mkslice(srcPtr, srcLength))
    }

    return count
}
