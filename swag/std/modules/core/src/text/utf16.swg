#global public
#global namespace utf16

const CharError = 0xFFFD'char
const Surr1     = 0xd800
const Surr2     = 0xdc00
const Surr3     = 0xe000
const SurrSelf  = 0x10000

// Get the unicode character pointed by 'buffer', and the number of u16 to encode it
// Will return 'CharError' for an invalid utf16 sequence
func decodeChar(buffer: const [..] u16) -> {c: char, eat: u32}
{
    var result: retval

    r := buffer[0]
    switch
    {
        case r < Surr1, r >= Surr3:
            result.c = cast(char) r
            result.eat = 1
            return result

        case Surr1 <= r && r < Surr2 && @countof(buffer) >= 2 && Surr2 <= buffer[1] && buffer[1] < Surr3:
             result.c = (r - Surr1) << 10 | (buffer[1] - Surr2) + SurrSelf
             result.eat = 2
             return result

        default:
            result.c = CharError
            return result
    }
}

// Convert unicode character 'src' to an utf16 sequence, and returns
// the number of u16 that were needed to make the conversion.
// 'dest' must be at least 2 u16 long
func fromChar(dest: [..] u16, src: char)->u32
{
    debug.assert(@countof(dest) >= 2, "destination buffer is too small")
    switch
    {
        case 0 <= src && src < Surr1, Surr3 <= src && src < SurrSelf:
            dest[0] = cast(u16) src
            return 1

        case SurrSelf <= src && src <= 0x10FFFF:
            r := src - SurrSelf
	        dest[0] = Surr1 + (r >> 10) & 0x3ff
            dest[1] = Surr2 + (r & 0x3ff)
            return 2
        default:
            dest[0] = cast(u16) CharError
            return 1
    }
}

// Convert an utf8 buffer to a utf16 buffer, and returns the
// number of valid elements in the destination buffer
func fromUtf8(dest: [..] u16, src: const [..] u8)->u32
{
    count := 0'u32
    srcPtr := @dataof(src)
    destPtr := @dataof(dest)
    srcLength := @countof(src)

    (c, eat) := utf8.decodeChar(src)
    while eat != 0
    {
        debug.assert(count != @countof(dest), "destination buffer is too small")
        debug.assert(c <= autocast swag.MaxU16, "utf8 character overflow")

        :destPtr = cast(u16) c
        destPtr, count += 1

        srcPtr += eat
        srcLength -= eat
        (c, eat) = utf8.decodeChar(@mkslice(srcPtr, srcLength))
    }

    return count
}

// Convert an utf8 buffer to an utf16 sequence
func fromUtf8(src: const [..] u8)->Array'u16
{
    var result: retval
    result.reserve(@countof(src))
    result.count = fromUtf8(@mkslice(result.buffer, @countof(src)), src)
    return result
}