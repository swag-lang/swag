#global public
#global namespace utf16

const RuneError = 0xFFFD'rune
const Surr1     = 0xd800
const Surr2     = 0xdc00
const Surr3     = 0xe000
const SurrSelf  = 0x10000

// Get the unicode character pointed by 'buffer', and the number of u16 to encode it
// Will return 'RuneError' for an invalid utf16 sequence
func decodeRune(buffer: const [..] u16) -> {c: rune, eat: u32}
{
    var result: retval

    r := cast(u32) buffer[0]
    switch
    {
        case r < Surr1, r >= Surr3:
            result.c = cast(rune) r
            result.eat = 1
            return result

        case Surr1 <= r and r < Surr2 and @countof(buffer) >= 2 and Surr2 <= buffer[1] and buffer[1] < Surr3:
            result.c = ((r - Surr1) << 10 | (buffer[1] - Surr2)) + SurrSelf
            result.eat = 2
            return result

        default:
            result.c = RuneError
            return result
    }
}

// Convert unicode character 'src' to an utf16 sequence, and returns
// the number of u16 that were needed to make the conversion.
// 'dest' must be at least 2 u16 long
func encodeRune(dest: [..] u16, src: rune)->u32
{
    Debug.assert(@countof(dest) >= 2, "destination buffer is too small")
    switch
    {
        case 0 <= src and src < Surr1, Surr3 <= src and src < SurrSelf:
            dest[0] = cast(u16) src
            return 1

        case SurrSelf <= src and src <= 0x10FFFF:
            r := src - SurrSelf
	        dest[0] = Surr1 + ((r >>,safe 10) & 0x3ff)
            dest[1] = Surr2 + (r & 0x3ff)
            return 2

        default:
            dest[0] = cast(u16) RuneError
            return 1
    }
}

// Convert an utf8 buffer to a utf16 buffer, and returns the
// number of valid elements in the destination buffer
func fromUtf8(dest: [..] u16, src: const [..] u8)->uint
{
    count := 0'uint
    srcPtr := @dataof(src)
    destPtr := @dataof(dest)
    srcLength := @countof(src)
    destLength := @countof(dest)

    while srcLength
    {
        (c, eat) := Utf8.decodeRune(@mkslice(srcPtr, srcLength))

        Debug.assert(destLength >= 2, "destination buffer is too small")
        eatDest := encodeRune(@mkslice(destPtr, destLength), c)
        destPtr, count += eatDest
        destLength -= eatDest

        srcPtr += eat
        srcLength -= eat
    }

    return count
}

// Convert an utf8 buffer to an utf16 sequence
func fromUtf8(src: const [..] u8)->Array'u16
{
    var result: retval
    result.reserve(2 * @countof(src))
    result.count = fromUtf8(@mkslice(result.buffer, result.capacity), src)
    return result
}

// Convert a character array (32 bits unicode) to an utf16 buffer
// Returns the number of bytes written in the destination buffer
// 'dest' must be at least 2 bytes long
func fromUnicode(dest: [..] u16, src: const [..] rune)->uint
{
    dataof := @dataof(dest)
    remain := @countof(dest)

    visit ch: src
    {
        Debug.assert(remain >= 2, "destination buffer is too small")
        cpt := encodeRune(@mkslice(dataof, remain), ch)
        dataof += cpt
        remain -= cpt
    }

    return cast(uint) (dataof - @dataof(dest))
}

// Convert a character array (32 bits unicode) to an utf16 sequence
func fromUnicode(src: const [..] rune)->Array'u16
{
    var result: retval
    result.reserve(2 * @countof(src))
    result.count = fromUnicode(@mkslice(result.buffer, result.capacity), src)
    return result
}

// Convert string to an utf16 array, zero terminated
func toZeroTerminated(src: string)->Array'u16
{
    var result: retval
    result.reserve(2 * @countof(src) + 1)
    result.count = utf16.fromUtf8(@mkslice(result.buffer, result.capacity), src) + 1
    result[result.count - 1] = 0
    return result
}

// Compute the string length of a zero terminated utf16 buffer
func lengthZeroTerminated(buf: const *u16)->uint
{
    ptr := buf
    count := 0'uint
    while dref ptr
        ptr, count += 1
    return count
}