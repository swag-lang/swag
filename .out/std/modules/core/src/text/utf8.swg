#global public
#global namespace utf8

const CharError     = 0xFFFD'char
const MaxChars      = 0x10FFFF'char
const SurrogateMin  = 0xD800'char
const SurrogateMax  = 0xDFFF'char

// https://github.com/golang/go/blob/master/src/unicode/utf8/utf8.go
protected
{
	const MaskX = 0b00111111
	const Mask2 = 0b00011111
	const Mask3 = 0b00001111
	const Mask4 = 0b00000111

	// The default lowest and highest continuation byte.
	const LoCB = 0b10000000
	const HiCB = 0b10111111

	// These names of these constants are chosen to give nice alignment in the
	// table below. The first nibble is an index into AcceptRanges or F for
	// special one-byte cases. The second nibble is the Rune length or the
	// Status for the special one-byte case.
	const XX = 0xF1 // invalid: size 1
	const AS = 0xF0 // ASCII: size 1
	const S1 = 0x02 // accept 0, size 2
	const S2 = 0x13 // accept 1, size 3
	const S3 = 0x03 // accept 0, size 3
	const S4 = 0x23 // accept 2, size 3
	const S5 = 0x34 // accept 3, size 4
	const S6 = 0x04 // accept 0, size 4
	const S7 = 0x44 // accept 4, size 4

    // first is information about the first byte in a UTF-8 sequence.
    const First: [256] u8 = @[
        //   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
        AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, // 0x00-0x0F
        AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, // 0x10-0x1F
        AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, // 0x20-0x2F
        AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, // 0x30-0x3F
        AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, // 0x40-0x4F
        AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, // 0x50-0x5F
        AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, // 0x60-0x6F
        AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, // 0x70-0x7F
        //   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
        XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, // 0x80-0x8F
        XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, // 0x90-0x9F
        XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, // 0xA0-0xAF
        XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, // 0xB0-0xBF
        XX, XX, S1, S1, S1, S1, S1, S1, S1, S1, S1, S1, S1, S1, S1, S1, // 0xC0-0xCF
        S1, S1, S1, S1, S1, S1, S1, S1, S1, S1, S1, S1, S1, S1, S1, S1, // 0xD0-0xDF
        S2, S3, S3, S3, S3, S3, S3, S3, S3, S3, S3, S3, S3, S4, S3, S3, // 0xE0-0xEF
        S5, S6, S6, S6, S7, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, // 0xF0-0xFF
    ]

    struct AcceptRange
    {
        lo: u8
        hi: u8
    }

    const AcceptRanges: [] AcceptRange = @[
        {LoCB, HiCB},
        {0xA0, HiCB},
        {LoCB, 0x9F},
        {0x90, HiCB},
        {LoCB, 0x8F},
    ]
}

// Get the last unicode character of the utf8 slice, and the number of
// bytes to encode it
func decodeLastChar(buffer: const [..] u8) -> {c: char, eat: u32}
{
    var result: retval

    countof := @countof(buffer)
    if !countof
    {
        result.c = CharError
        return result
    }

    dataof := @dataof(buffer)
    ptr := dataof + (countof - 1)

    // Ascii
    c := cast(char) :ptr
    if c & 0x80 == 0
    {
        result.c = c
        result.eat = 1
        return result
    }

    eat := 1'u32
    while true
    {
        c = :ptr
        if c >>% 6 != 2
            break
        if ptr == dataof
        {
            result.c = CharError
            result.eat = 1
            return result
        }

        ptr -= 1
        eat += 1
        if eat > 4
        {
            result.c = CharError
            result.eat = 1
            return result
        }
    }

    result = decodeChar(@mkslice(ptr, cast(uint) eat))
    if result.eat != eat
    {
        result.c = CharError
        result.eat = 1
    }

    return result
}

// Get the unicode character pointed by 'buffer', and the number of bytes to encode it
// Will return 'CharError' for an invalid utf8 sequence
func decodeChar(buffer: const [..] u8) -> {c: char, eat: u32}
{
    var result: retval

    countof := @countof(buffer)
    if countof == 0
    {
        result.c = CharError
        return result
    }

    dataof := @dataof(buffer)
    p0 := cast(char) :dataof

    // Ascii
    if (p0 & 0x80) == 0
    {
        result.c = p0
        result.eat = 1
        return result
    }

    x := First[cast(u8) p0]

    // Invalid first byte
    if x == XX
    {
        result.c = CharError
        result.eat = 1
        return result
    }

    // Be sure input buffer is large enough to hold the requested
    // encoded size
    sz := x & 7
	if countof < sz
    {
        result.c = CharError
        result.eat = 1
        return result
    }

	accept := AcceptRanges[x >>% 4]

    // Length == 2
	b1 := dataof[1]
	if !(accept.lo <= b1 && b1 <= accept.hi)
    {
        result.c = CharError
        result.eat = 1
        return result
    }

    if sz <= 2
    {
        result.c = (p0 & Mask2) << 6 | (b1 & MaskX)
        result.eat = 2
        return result
    }

    // Length == 3
	b2 := dataof[2]
    if !(LoCB <= b2 && b2 <= HiCB)
    {
        result.c = CharError
        result.eat = 1
        return result
    }

	if sz <= 3
    {
        result.c = (p0 & Mask3) << 12 | (b1 & MaskX) << 6 | (b2 & MaskX)
        result.eat = 3
        return result
    }

    // Length == 4
	b3 := dataof[3]
	if !(LoCB <= b3 && b3 <= HiCB)
    {
        result.c = CharError
        result.eat = 1
        return result
    }

	result.c = (p0 & Mask4) << 18 | (b1 & MaskX) << 12 | (b2 & MaskX) << 6 | (b3 & MaskX)
    result.eat = 4
    return result
}

// Convert unicode character 'src' to an utf8 sequence, and returns
// the number of bytes that were needed to make the conversion.
// 'dest' must be at least 4 bytes long
func encodeChar(dest: [..] u8, src: char)->u32
{
    debug.assert(@countof(dest) >= 4, "destination buffer is too small")

    switch
    {
    case src <= 0x7F:
        dest[0] = cast(u8) src
        return 1

    case src <= 0x07FF:
        dest[0] = cast(u8) (((src >>% 6) & 0x1F) | 0xC0)
        dest[1] = cast(u8) (((src >>% 0) & 0x3F) | 0x80)
        return 2

    case !isValidChar(src):
        dest[0] = cast(u8) (((CharError >>% 12) & 0x0F) | 0xE0)
        dest[1] = cast(u8) (((CharError >>% 6) & 0x3F) | 0x80)
        dest[2] = cast(u8) (((CharError >>% 0) & 0x3F) | 0x80)
        return 3

    case src <= 0xFFFF:
        dest[0] = cast(u8) (((src >>% 12) & 0x0F) | 0xE0)
        dest[1] = cast(u8) (((src >>% 6) & 0x3F) | 0x80)
        dest[2] = cast(u8) (((src >>% 0) & 0x3F) | 0x80)
        return 3

    default:
        dest[0] = cast(u8) (((src >>% 18) & 0x07) | 0xF0)
        dest[1] = cast(u8) (((src >>% 12) & 0x3F) | 0x80)
        dest[2] = cast(u8) (((src >>% 6) & 0x3F) | 0x80)
        dest[3] = cast(u8) (((src >>% 0) & 0x3F) | 0x80)
        return 4
    }
}

// Convert a character array (32 bits unicode) to an utf8 buffer
// Returns the number of bytes written in the destination buffer
// 'dest' must be at least 4 bytes long
func fromUnicode(dest: [..] u8, src: const [..] char)->uint
{
    dataof := @dataof(dest)
    remain := @countof(dest)

    total := 0'uint
    visit ch: src
    {
        debug.assert(remain >= 4, "destination buffer is too small")
        slice := @mkslice(dataof + total, remain)
        cpt := encodeChar(slice, ch)
        total += cpt
        remain -= cpt
    }

    return total
}

// Convert an unicode buffer to a String
func fromUnicode(src: const [..] char)->String
{
    var result: retval
    result.reserve(4 * @countof(src))
    result.length = fromUnicode(@mkslice(result.buffer, result.capacity), src)
    return result
}

// Convert an utf16 array to an utf8 buffer
// Returns the number of bytes written in the destination buffer
// 'dest' must be at least 4 bytes long
func fromUtf16(dest: [..] u8, src: const [..] u16)->uint
{
    count := 0'uint
    srcPtr := @dataof(src)
    destPtr := @dataof(dest)
    srcLength := @countof(src)
    destLength := @countof(dest)

    while srcLength
    {
        (c, eat) := utf16.decodeChar(@mkslice(srcPtr, srcLength))

        debug.assert(destLength >= 4, "destination buffer is too small")
        eatDest := encodeChar(@mkslice(destPtr, destLength), c)
        destPtr, count += eatDest
        destLength -= eatDest

        srcPtr += eat
        srcLength -= eat
    }

    return count
}

// Convert an utf16 buffer to a String
func fromUtf16(src: const [..] u16)->String
{
    var result: retval
    result.reserve(4 * @countof(src))
    result.length = fromUtf16(@mkslice(result.buffer, result.capacity), src)
    return result
}

// Returns true if the given unicode character can be encoded in utf8
func isValidChar(c: char)->bool
{
	switch
    {
	case 0 <= c && c < SurrogateMin:
		return true
	case SurrogateMax < c && c <= MaxChars:
		return true
	}

	return false
}
