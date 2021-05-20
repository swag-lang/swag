// https://github.com/golang/go/blob/master/src/unicode/utf8/utf8.go

#global public
#global namespace utf8

const RuneError     = 0xFFFD'rune
const MaxRunes      = 0x10FFFF'rune
const SurrogateMin  = 0xD800'rune
const SurrogateMax  = 0xDFFF'rune

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

// Get the last unicode rune of the utf8 slice, and the number of
// bytes to encode it
func decodeLastRune(buffer: const [..] u8) -> {c: rune, eat: u32}
{
    var result: retval

    countof := @countof(buffer)
    if !countof
    {
        result.c = RuneError
        return result
    }

    dataof := @dataof(buffer)
    ptr := dataof + (countof - 1)

    // Ascii
    c := cast(rune) dref ptr
    if c & 0x80 == 0
    {
        result.c = c
        result.eat = 1
        return result
    }

    eat := 1'u32
    loop
    {
        c = dref ptr
        if c >>,safe 6 != 2
            break
        if ptr == dataof
        {
            result.c = RuneError
            result.eat = 1
            return result
        }

        ptr -= 1
        eat += 1
        if eat > 4
        {
            result.c = RuneError
            result.eat = 1
            return result
        }
    }

    result = decodeRune(@mkslice(ptr, cast(uint) eat))
    if result.eat != eat
    {
        result.c = RuneError
        result.eat = 1
    }

    return result
}

// Get the unicode rune pointed by 'buffer', and the number of bytes to encode it
// Will return 'RuneError' for an invalid utf8 sequence
func decodeRune(buffer: const [..] u8) -> {c: rune, eat: u32}
{
    var result: retval

    countof := @countof(buffer)
    if countof == 0
    {
        result.c = RuneError
        return result
    }

    dataof := @dataof(buffer)
    p0 := cast(rune) dref dataof

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
        result.c = RuneError
        result.eat = 1
        return result
    }

    // Be sure input buffer is large enough to hold the requested
    // encoded size
    sz := x & 7
	if countof < sz
    {
        result.c = RuneError
        result.eat = 1
        return result
    }

	accept := AcceptRanges[x >>,safe 4]

    // Length == 2
	b1 := dataof[1]
	if !(accept.lo <= b1 and b1 <= accept.hi)
    {
        result.c = RuneError
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
    if !(LoCB <= b2 and b2 <= HiCB)
    {
        result.c = RuneError
        result.eat = 1
        return result
    }

	if sz <= 3
    {
        result.c = (p0 & Mask3) << 12 | cast(rune) (b1 & MaskX) << 6 | (b2 & MaskX)
        result.eat = 3
        return result
    }

    // Length == 4
	b3 := dataof[3]
	if !(LoCB <= b3 and b3 <= HiCB)
    {
        result.c = RuneError
        result.eat = 1
        return result
    }

	result.c = (p0 & Mask4) << 18 | cast(rune) (b1 & MaskX) << 12 | cast(rune) (b2 & MaskX) << 6 | (b3 & MaskX)
    result.eat = 4
    return result
}

// Convert rune 'src' to an utf8 sequence, and returns
// the number of bytes that were needed to make the conversion.
// 'dest' must be at least 4 bytes long
func encodeRune(dest: [..] u8, src: rune)->u32
{
    Debug.assert(@countof(dest) >= 4, "destination buffer is too small")

    switch
    {
    case src <= 0x7F:
        dest[0] = cast(u8) src
        return 1

    case src <= 0x07FF:
        dest[0] = cast(u8) (((src >>,safe 6) & 0x1F) | 0xC0)
        dest[1] = cast(u8) (((src >>,safe 0) & 0x3F) | 0x80)
        return 2

    case !isValidRune(src):
        dest[0] = cast(u8) (((RuneError >>,safe 12) & 0x0F) | 0xE0)
        dest[1] = cast(u8) (((RuneError >>,safe 6) & 0x3F) | 0x80)
        dest[2] = cast(u8) (((RuneError >>,safe 0) & 0x3F) | 0x80)
        return 3

    case src <= 0xFFFF:
        dest[0] = cast(u8) (((src >>,safe 12) & 0x0F) | 0xE0)
        dest[1] = cast(u8) (((src >>,safe 6) & 0x3F) | 0x80)
        dest[2] = cast(u8) (((src >>,safe 0) & 0x3F) | 0x80)
        return 3

    default:
        dest[0] = cast(u8) (((src >>,safe 18) & 0x07) | 0xF0)
        dest[1] = cast(u8) (((src >>,safe 12) & 0x3F) | 0x80)
        dest[2] = cast(u8) (((src >>,safe 6) & 0x3F) | 0x80)
        dest[3] = cast(u8) (((src >>,safe 0) & 0x3F) | 0x80)
        return 4
    }
}

// Convert a rune array to an utf8 buffer
// Returns the number of bytes written in the destination buffer
// 'dest' must be at least 4 bytes long
func fromUnicode(dest: [..] u8, src: const [..] rune)->uint
{
    dataof := @dataof(dest)
    remain := @countof(dest)

    total := 0'uint
    visit ch: src
    {
        Debug.assert(remain >= 4, "destination buffer is too small")
        sl := @mkslice(dataof + total, remain)
        cpt := encodeRune(sl, ch)
        total += cpt
        remain -= cpt
    }

    return total
}

// Convert an unicode buffer to a String
func fromUnicode(src: const [..] rune)->String
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
        (c, eat) := utf16.decodeRune(@mkslice(srcPtr, srcLength))

        Debug.assert(destLength >= 4, "destination buffer is too small")
        eatDest := encodeRune(@mkslice(destPtr, destLength), c)
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

// Returns true if the given unicode rune can be encoded in utf8
func isValidRune(c: rune)->bool
{
	switch
    {
	case 0 <= c and c < SurrogateMin:
		return true
	case SurrogateMax < c and c <= MaxRunes:
		return true
	}

	return false
}


// Returns the number of bytes to encode the first rune of the
// utf8 buffer.
// If it's an invalid encoding, returns 1.
func countBytesAt(buffer: const [..] u8)->u32
{
    dataof := @dataof(buffer)
    countof := @countof(buffer)
    if !countof
        return 1

    p0 := cast(rune) dref dataof

    // Ascii
    if (p0 & 0x80) == 0
        return 1

    // Invalid first byte
    x := First[cast(u8) p0]
    if x == XX
        return 1

    // Be sure input buffer is large enough to hold the requested
    // encoded size
    sz := x & 7
	if countof < sz
        return 1

	accept := AcceptRanges[x >>,safe 4]

	b1 := dataof[1]
	if !(accept.lo <= b1 and b1 <= accept.hi)
        return 1
	if sz <= 2
        return 2

	b2 := dataof[2]
    if !(LoCB <= b2 and b2 <= HiCB)
        return 1
	if sz <= 3
        return 3

	b3 := dataof[3]
	if !(LoCB <= b3 and b3 <= HiCB)
        return 1
	return 4
}

// Returns the byte index of the given rune index
func byteIndex(buffer: const [..] u8, charIndex: uint) -> {index: uint, success: bool}
{
    var result: retval

    countof := @countof(buffer)
    if !countof
        return result

    if !charIndex
    {
        result.success = true
        return result
    }

    dataof := @dataof(buffer)

    resultChar := 0'uint
    resultByte := 0'uint
    while countof and resultChar != charIndex
    {
        val := countBytesAt(@mkslice(dataof, countof))
        Debug.assert(val <= countof, "invalid utf8 character")
        resultByte += val
        resultChar += 1
        dataof += val
        countof -= val
    }

    result.index = resultByte
    result.success = resultByte < @countof(buffer)
    return result
}

// Returns the number of runes in an utf8 buffer
func countRunes(buffer: const [..] u8)->uint
{
    dataof := @dataof(buffer)
    countof := @countof(buffer)

    cpt := 0'uint
    while countof
    {
        val := countBytesAt(@mkslice(dataof, countof))
        Debug.assert(val <= countof)
        cpt += 1
        dataof += val
        countof -= val
    }

    return cpt
}

// Find the first occurence of rune 'what', and returns the byte index of it
// Returns Swag.UInt.Max if not found
func indexOf(src: const [..] u8, what: rune, startByteIndex = 0'uint)->uint
{
    return indexOf(src, @[what], startByteIndex)
}

// Find one of the runes in 'what', and returns the byte index of it
// Returns Swag.UInt.Max if not found
func indexOf(src: const [..] u8, what: const [..] rune, startByteIndex = 0'uint)->uint
{
    length := @countof(src)
    if !length
        return Swag.UInt.Max
    Debug.assert(startByteIndex < length, "invalid 'startByteIndex'")

    countof := @countof(what)
    if !countof
        return Swag.UInt.Max

    idx := startByteIndex
    scan := @dataof(src) + startByteIndex
    remain := length - startByteIndex
    while countof <= remain
    {
        res := utf8.decodeRune(@mkslice(scan, remain))
        if !res.eat
            break
        if Slice.contains(what, res.c)
            return idx
        scan, idx += res.eat
        remain -= res.eat
    }

    return Swag.UInt.Max
}

// Find the given string, and returns the byte index of it
// Returns Swag.UInt.Max if not found
func indexOf(src: const [..] u8, what: string, startByteIndex = 0'uint, comparisonType = ComparisonType.Latin1)->uint
{
    length := @countof(src)
    if !length
        return Swag.UInt.Max
    Debug.assert(startByteIndex < length, "invalid 'startByteIndex'")

    countof := @countof(what)
    if !countof or countof > (length - startByteIndex)
        return Swag.UInt.Max

    idx := startByteIndex
    scan := @dataof(src) + startByteIndex
    remain := length - startByteIndex
    while countof <= remain
    {
        res := compare(@mkslice(scan, countof), what, comparisonType)
        if res == 0
            return idx
        scan, idx += 1
        remain -= 1
    }

    return Swag.UInt.Max
}

// Find the last rune occurence of 'what', and returns the byte index of it
// Returns Swag.UInt.Max if not found
func lastIndexOf(src: const [..] u8, what: rune)->uint
{
    return lastIndexOf(src, @[what])
}

// Returns the last index (in bytes) of a rune
// Returns Swag.UInt.Max if not found
func lastIndexOf(src: const [..] u8, what: const [..] rune)->uint
{
    length := @countof(src)
    if !length
        return Swag.UInt.Max
    dataof := @dataof(src)

    byteIdx := length
    loop
    {
        sl := @mkslice(dataof, byteIdx)
        (c, eat) := decodeLastRune(sl)
        if eat == 0
            return Swag.UInt.Max

        byteIdx -= eat
        if Slice.contains(what, c)
            return byteIdx
    }

    return Swag.UInt.Max
}

// Returns the last index (in bytes) of a string
// Returns Swag.UInt.Max if not found
func lastIndexOf(src: const [..] u8, what: string, comparisonType = ComparisonType.Latin1)->uint
{
    length := @countof(src)
    if !length
        return Swag.UInt.Max
    lengthStr := @countof(what)
    if lengthStr > length
        return Swag.UInt.Max

    idx := length - lengthStr
    dataof := @dataof(src) + idx
    loop
    {
        res := compare(@mkslice(dataof, lengthStr), what, comparisonType)
        if res == 0
            return idx
        if !idx
            return Swag.UInt.Max
        idx, dataof -= 1
    }

    return Swag.UInt.Max
}

// Return true if the string starts with 'str'
func startsWith(src: const [..] u8, str: string, comparisonType = ComparisonType.Latin1)->bool
{
    length := @countof(src)
    countof := @countof(str)
    if countof > length
        return false
    buffer := @dataof(src)
    return compare(@mkslice(buffer, countof), str, comparisonType) == 0
}

// Return true if the string ends with 'str'
func endsWith(src: const [..] u8, str: string, comparisonType = ComparisonType.Latin1)->bool
{
    length := @countof(src)
    countof := @countof(str)
    if countof > length
        return false
    buffer := @dataof(src)
    return compare(@mkslice((buffer + length) - countof, countof), str, comparisonType) == 0
}

// Returns the first rune of the slice
func firstRune(src: const [..] u8)->rune
{
    countof := @countof(src)
    if !countof
        return 0
    dataof := @dataof(src)
    if latin1.isAscii(dataof[0])
        return cast(rune) dataof[0]
    (c, ?) := utf8.decodeRune(src)
    return c
}

// Returns the last rune of the slice
func lastRune(src: const [..] u8)->rune
{
    countof := @countof(src)
    if !countof
        return 0
    dataof := @dataof(src)
    if latin1.isAscii(dataof[countof - 1])
        return cast(rune) dataof[countof - 1]
    (c, ?) := utf8.decodeLastRune(src)
    return c
}

public enum ComparisonType
{
    Latin1
    Latin1NoCase
    Unicode
    UnicodeNoCase
}

// Compare two utf8 buffers with the given algorithm
func compare(src, dst: const [..] u8, comparisonType = ComparisonType.Latin1)->s32
{
    using ComparisonType

    #[Swag.complete]
    switch comparisonType
    {
    case Latin1:
        return latin1.compare(src, dst) // bytes compare

    case Latin1NoCase:
        return latin1.compare(src, dst, ignoreCase: true)

    case Unicode:
        return latin1.compare(src, dst) // bytes compare

    case UnicodeNoCase:
        length := @countof(src)
        otherLength := @countof(dst)
        data := @dataof(src)
        otherData := @dataof(dst)

        var result: s32
        var c1, c2: rune
        var eat1, eat2: u32
        loop
        {
            (c1, eat1) = decodeRune(@mkslice(data, length))
            (c2, eat2) = decodeRune(@mkslice(otherData, otherLength))
            if eat1 == 0 or eat2 == 0
                break

            if c1 != c2
            {
                c1 = unicode.toLower(c1)
                c2 = unicode.toLower(c2)
                result = cast(s32) c1 - cast(s32) c2
                if result != 0
                    return math.sign(result)
            }

            length -= eat1
            otherLength -= eat2
            data += eat1
            otherData += eat2
        }

        if !eat1 and eat2
            return -1
        if eat1 and !eat2
            return 1
        return 0
    }

    return 0
}

// Macro to visit the unicode characters of the utf8 sequence
// @alias0 will contain the character
// @alias1 will contain the character index
// @alias2 will contain the byte index of that character within the utf8 sequence
#[Swag.macro]
func visitRunes(buffer: const [..] u8, stmt: code)
{
    count := @countof(buffer)
    if !count
        return

    byteIdx := 0'uint
    charIdx := 0'uint
    scan := @dataof(buffer)
    while count
    {
        sl := @mkslice(scan, count)
        (c, eat) := decodeRune(sl)
        if eat == 0
            break
        #macro
        {
            @alias0 := `c
            @alias1 := `charIdx
            @alias2 := `byteIdx
            #mixin `stmt
        }

        charIdx += 1
        byteIdx += eat
        scan += eat
        count -= eat
    }
}

// Returns true if the utf8 sequence is valid
func isValid(buffer: const [..] u8)->bool
{
    p := @dataof(buffer)
    countof := @countof(buffer)
    if !countof
        return true

    // Quick zap ascii characters
    while countof >= 8
    {
        first64 := dref cast(const *u64) p
        if first64 & 0x80808080_80808080
            break
        p += 8
        countof -= 8
    }

    while countof
    {
        p0 := cast(rune) dref p

        // Ascii
        if (p0 & 0x80) == 0
        {
            p += 1
            countof -= 1
            continue
        }

        // Invalid first byte
        x := First[cast(u8) p0]
        if x == XX
            return false

        // Be sure input buffer is large enough to hold the requested
        // encoded size
        sz := x & 7
        if countof < sz
            return false

        accept := AcceptRanges[x >>,safe 4]

        b1 := p[1]
        if !(accept.lo <= b1 and b1 <= accept.hi)
            return false
        if sz <= 2
        {
            p += 2
            countof -= 2
            continue
        }

        b2 := p[2]
        if !(LoCB <= b2 and b2 <= HiCB)
            return false
        if sz <= 3
        {
            p += 3
            countof -= 3
            continue
        }

        b3 := p[3]
        if !(LoCB <= b3 and b3 <= HiCB)
            return false
        p += 4
        countof -= 4
    }

    return true
}
