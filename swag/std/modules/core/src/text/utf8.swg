#global public
#global namespace utf8

const CharError     = 0xFFFD'char
const MaxChars      = 0x10FFFF'char
const SurrogateMin  = 0xD800'char
const SurrogateMax  = 0xDFFF'char

// https://github.com/golang/go/blob/master/src/unicode/utf8/utf8.go
private
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

// Returns the number of bytes to encode the first character of the
// utf8 buffer.
// If it's an invalid encoding, returns 1.
func countBytesAt(buffer: const [..] u8)->u32
{
    dataof := @dataof(buffer)
    countof := @countof(buffer)
    if !countof
        return 1

    p0 := cast(char) :dataof

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

	accept := AcceptRanges[x >> 4]

	b1 := dataof[1]
	if !(accept.lo <= b1 <= accept.hi)
        return 1
	if sz <= 2
        return 2

	b2 := dataof[2]
    if !(LoCB <= b2 <= HiCB)
        return 1
	if sz <= 3
        return 3

	b3 := dataof[3]
	if !(LoCB <= b3 <= HiCB)
        return 1
	return 4
}

// Returns the byte index of the given character index
func byteIndex(buffer: const [..] u8, charIndex: uint) -> {index: uint, success: bool}
{
    countof := @countof(buffer)
    if !countof
        return @{0'u32, false}
    if !charIndex
        return @{0'u32, true}

    dataof := @dataof(buffer)

    resultChar := 0'uint
    resultByte := 0'uint
    while countof && resultChar != charIndex
    {
        val := countBytesAt(@mkslice(dataof, countof))
        debug.assert(val <= countof, "invalid utf8 character")
        resultByte += val
        resultChar += 1
        dataof += val
        countof -= val
    }

    return @{resultByte, resultByte < @countof(buffer)}
}

// Returns the number of unicode characters in an utf8 buffer
func countChars(buffer: const [..] u8)->uint
{
    dataof := @dataof(buffer)
    countof := @countof(buffer)

    cpt := 0'uint
    while countof
    {
        val := countBytesAt(@mkslice(dataof, countof))
        debug.assert(val <= countof)
        cpt += 1
        dataof += val
        countof -= val
    }

    return cpt
}

// Get the last utf8 character of the slice, and the number of bytes to encode it
func lastChar(buffer: const [..] u8) -> {c: char, eat: u32}
{
    countof := @countof(buffer)
    if !countof
        return @{CharError, 0}

    dataof := @dataof(buffer)
    ptr := dataof + (countof - 1)

    // Ascii
    c := cast(char) :ptr
    if c & 0x80 == 0
        return @{c, 1}

    eat := 1'u32
    while true
    {
        c = :ptr
        if c >> 6 != 2
            break
        if ptr == dataof
            return @{CharError, 1}
        ptr -= 1
        eat += 1
        if eat > 4
            return @{CharError, 1}
    }

    var result: retval
    result = firstChar(@mkslice(ptr, cast(uint) eat))
    if result.eat != eat
        return @{CharError, 1}
    return result
}

// Get the unicode character pointed by 'buffer', and the number of bytes to encode it
// Will return 'CharError' for an invalid utf8 sequence
func firstChar(buffer: const [..] u8) -> {c: char, eat: u32}
{
    countof := @countof(buffer)
    if countof == 0
        return @{CharError, 0}

    dataof := @dataof(buffer)
    p0 := cast(char) :dataof

    // Ascii
    if (p0 & 0x80) == 0
        return @{p0, 1}

    x := First[cast(u8) p0]

    // Invalid first byte
    if x == XX
        return @{CharError, 1}

    // Be sure input buffer is large enough to hold the requested
    // encoded size
    sz := x & 7
	if countof < sz
        return @{CharError, 1}

	accept := AcceptRanges[x >> 4]

    // Length == 2
	b1 := dataof[1]
	if !(accept.lo <= b1 <= accept.hi)
        return @{CharError, 1}
	if sz <= 2
        return @{(p0 & Mask2) << 6 | (b1 & MaskX), 2}

    // Length == 3
	b2 := dataof[2]
    if !(LoCB <= b2 <= HiCB)
        return @{CharError, 1}
	if sz <= 3
        return @{(p0 & Mask3) << 12 | (b1 & MaskX) << 6 | (b2 & MaskX), 3}

    // Length == 4
	b3 := dataof[3]
	if !(LoCB <= b3 <= HiCB)
        return @{CharError, 1}
	return @{(p0 & Mask4) << 18 | (b1 & MaskX) << 12 | (b2 & MaskX) << 6 | (b3 & MaskX), 4}
}

// Find the first occurence of character 'what', and returns the byte index of it
// Returns swag.MaxUINT if not found
func indexOf(src: const [..] u8, what: char, startByteIndex = 0'uint)->uint
{
    return indexOf(src, @[what], startByteIndex)
}

// Find one of the characters in 'what', and returns the byte index of it
// Returns swag.MaxUINT if not found
func indexOf(src: const [..] u8, what: const [..] char, startByteIndex = 0'uint)->uint
{
    length := @countof(src)
    if !length
        return swag.MaxUINT
    debug.assert(startByteIndex < length, "invalid 'startByteIndex'")

    countof := @countof(what)
    if !countof
        return swag.MaxUINT

    idx := startByteIndex
    scan := @dataof(src) + startByteIndex
    remain := length - startByteIndex
    while countof <= remain
    {
        res := utf8.firstChar(cast(string) @mkslice(scan, remain))
        if !res.eat
            break
        if contains(what, res.c)
            return idx
        scan, idx += res.eat
        remain -= res.eat
    }

    return swag.MaxUINT
}

// Find the given string, and returns the byte index of it
// Returns swag.MaxUINT if not found
func indexOf(src: const [..] u8, what: string, startByteIndex = 0'uint, comparisonType = ComparisonType.Latin1)->uint
{
    length := @countof(src)
    if !length
        return swag.MaxUINT
    debug.assert(startByteIndex < length, "invalid 'startByteIndex'")

    countof := @countof(what)
    if !countof || countof > (length - startByteIndex)
        return swag.MaxUINT

    idx := startByteIndex
    scan := @dataof(src) + startByteIndex
    remain := length - startByteIndex
    while countof <= remain
    {
        res := compare(cast(string) @mkslice(scan, countof), what, comparisonType)
        if res == 0
            return idx
        scan, idx += 1
        remain -= 1
    }

    return swag.MaxUINT
}

// Find the last character occurence of 'what', and returns the byte index of it
// Returns swag.MaxUINT if not found
func lastIndexOf(src: const [..] u8, what: char)->uint
{
    return lastIndexOf(src, @[what])
}

// Returns the last index (in bytes) of a character
// Returns swag.MaxUINT if not found
func lastIndexOf(src: const [..] u8, what: const [..] char)->uint
{
    length := @countof(src)
    if !length
        return swag.MaxUINT
    dataof := @dataof(src)

    byteIdx := length
    while true
    {
        slice := @mkslice(dataof, byteIdx)
        (c, eat) := lastChar(slice)
        if eat == 0
            return swag.MaxUINT

        byteIdx -= eat
        if contains(what, c)
            return byteIdx
    }

    return swag.MaxUINT
}

// Return true if the string starts with 'str'
func startsWith(src: const [..] u8, str: string, comparisonType = utf8.ComparisonType.Latin1)->bool
{
    length := @countof(src)
    countof := @countof(str)
    if countof > length
        return false
    buffer := @dataof(src)
    return compare(cast(string) @mkslice(buffer, countof), str, comparisonType) == 0
}

// Return true if the string ends with 'str'
func endsWith(src: const [..] u8, str: string, comparisonType = utf8.ComparisonType.Latin1)->bool
{
    length := @countof(src)
    countof := @countof(str)
    if countof > length
        return false
    buffer := @dataof(src)
    return compare(cast(string) @mkslice((buffer + length) - countof, countof), str, comparisonType) == 0
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

    #[swag.complete]
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
        var c1, c2: char
        var eat1, eat2: u32
        while true
        {
            (c1, eat1) = firstChar(@mkslice(data, length))
            (c2, eat2) = firstChar(@mkslice(otherData, otherLength))
            if eat1 == 0 || eat2 == 0
                break

            if c1 != c2
            {
                c1 = unicode32.toLower(c1)
                c2 = unicode32.toLower(c2)
                result = cast(s32) c1 - cast(s32) c2
                if result != 0
                    return math.sign(result)
            }

            length -= eat1
            otherLength -= eat2
            data += eat1
            otherData += eat2
        }

        if !eat1 && eat2
            return -1
        if eat1 && !eat2
            return 1
        return 0
    }

    return 0
}

// Macro to visit the unicode characters of the utf8 sequence
// @alias0 will contain the character
// @alias1 will contain the byte index of that character within the utf8 sequence
// @alias2 will contain the character index
#[swag.macro]
func visitChars(buffer: const [..] u8, stmt: code)
{
    count := @countof(buffer)
    if !count
        return

    byteIdx := 0'uint
    charIdx := 0'uint
    scan := @dataof(buffer)
    while count
    {
        slice := @mkslice(scan, count)
        (c, eat) := firstChar(slice)
        if eat == 0
            break
        #macro
        {
            @alias0 := `c
            @alias1 := `byteIdx
            @alias2 := `charIdx
            #[swag.noreturn]
            #mixin `stmt
        }

        charIdx += 1
        byteIdx += eat
        scan += eat
        count -= eat
    }
}

// Convert unicode character 'src' to an utf8 sequence, and returns
// the number of bytes that were needed to make the conversion.
// 'dest' must be at least 4 bytes long
//
// For speed reasons, this function never fails, even if 'src' is invalid.
// Use 'fromCharSafe' if you're not sure of the validity of 'src'
func fromChar(dest: [..] u8, src: char)->u32
{
    debug.assert(@countof(dest) >= 4, "destination buffer is too small")

    switch
    {
    case src <= 0x7F:
        dest[0] = cast(u8) src
        return 1

    case src <= 0x07FF:
        dest[0] = cast(u8) (((src >> 6) & 0x1F) | 0xC0)
        dest[1] = cast(u8) (((src >> 0) & 0x3F) | 0x80)
        return 2

    case !isValidChar(src):
        dest[0] = cast(u8) (((CharError >> 12) & 0x0F) | 0xE0)
        dest[1] = cast(u8) (((CharError >> 6) & 0x3F) | 0x80)
        dest[2] = cast(u8) (((CharError >> 0) & 0x3F) | 0x80)
        return 3

    case src <= 0xFFFF:
        dest[0] = cast(u8) (((src >> 12) & 0x0F) | 0xE0)
        dest[1] = cast(u8) (((src >> 6) & 0x3F) | 0x80)
        dest[2] = cast(u8) (((src >> 0) & 0x3F) | 0x80)
        return 3

    default:
        dest[0] = cast(u8) (((src >> 18) & 0x07) | 0xF0)
        dest[1] = cast(u8) (((src >> 12) & 0x3F) | 0x80)
        dest[2] = cast(u8) (((src >> 6) & 0x3F) | 0x80)
        dest[3] = cast(u8) (((src >> 0) & 0x3F) | 0x80)
        return 4
    }
}

// Convert a character array (32 bits unicode) to an utf8 buffer
// Returns the number of bytes written in the destination buffer
// 'dest' must be at least 4 bytes long
func fromCharArray(dest: [..] u8, src: const [..] char)->uint
{
    dataof := @dataof(dest)
    remain := @countof(dest)

    total := 0'uint
    visit ch: src
    {
        debug.assert(remain >= 4, "destination buffer is too small")
        slice := @mkslice(dataof + total, remain)
        cpt := fromChar(slice, ch)
        total += cpt
        remain -= cpt
    }

    return total
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
        first64 := :cast(const *u64) p
        if first64 & 0x80808080_80808080
            break
        p += 8
        countof -= 8
    }

    while countof
    {
        p0 := cast(char) :p

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

        accept := AcceptRanges[x >> 4]

        b1 := p[1]
        if !(accept.lo <= b1 <= accept.hi)
            return false
        if sz <= 2
        {
            p += 2
            countof -= 2
            continue
        }

        b2 := p[2]
        if !(LoCB <= b2 <= HiCB)
            return false
        if sz <= 3
        {
            p += 3
            countof -= 3
            continue
        }

        b3 := p[3]
        if !(LoCB <= b3 <= HiCB)
            return false
        p += 4
        countof -= 4
    }

    return true
}

// Returns true if the given unicode character can be encoded in utf8
func isValidChar(c: char)->bool
{
	switch
    {
	case 0 <= c < SurrogateMin:
		return true
	case SurrogateMax < c <= MaxChars:
		return true
	}

	return false
}