public namespace std.unicode16;

// Convert an utf8 buffer to a 16 bits unicode sequence
func fromUtf8(dest: [..] u16, src: const [..] u8)->u32
{
    count := 0'u32
    result := utf8.nextChar(src)
    destPtr := @dataof(dest)
    while result.item1 != 0
    {
        debug.assert(count != @countof(dest), "destination buffer is too small")
        destPtr[0] = cast(u16) result.item1
        destPtr += 1
        count += 1
        result = utf8.nextChar(result.item0)
    }

    return count
}
