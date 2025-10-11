/*
    :LICENCE
    This code is based on the Go library (https://github.com/golang/go/blob/master/src/unicode/utf8/Utf8.go)
    The original code has been modified for Swag
    See LICENCE.md for the corresponding licence.
*/

#global public
#global namespace Utf8

const RuneError    = 0xFFFD'rune
const MaxRunes     = 0x10FFFF'rune
const SurrogateMin = 0xD800'rune
const SurrogateMax = 0xDFFF'rune

internal
{
    const MaskX = 0b00111111'u8
    const Mask2 = 0b00011111'u8
    const Mask3 = 0b00001111'u8
    const Mask4 = 0b00000111'u8

    // The default lowest and highest continuation byte.
    const LoCB = 0b10000000'u8
    const HiCB = 0b10111111'u8

    // These names of these constants are chosen to give nice alignment in the
    // table below. The first nibble is an index into AcceptRanges or F for
    // special one-byte cases. The second nibble is the Rune length or the

    // Status for the special one-byte case.
    const XX = 0xF1     // invalid: size 1
    const AS = 0xF0     // ASCII: size 1
    const S1 = 0x02     // accept 0, size 2
    const S2 = 0x13     // accept 1, size 3
    const S3 = 0x03     // accept 0, size 3
    const S4 = 0x23     // accept 2, size 3
    const S5 = 0x34     // accept 3, size 4
    const S6 = 0x04     // accept 0, size 4
    const S7 = 0x44     // accept 4, size 4

    // first is information about the first byte in a UTF-8 sequence.
    const First: [256] u8 = [
        //   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
        AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS,
        // 0x00-0x0F
        AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS,
        // 0x10-0x1F
        AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS,
        // 0x20-0x2F
        AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS,
        // 0x30-0x3F
        AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS,
        // 0x40-0x4F
        AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS,
        // 0x50-0x5F
        AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS,
        // 0x60-0x6F
        AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS, AS,
        // 0x70-0x7F
        //   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
        XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX,
        // 0x80-0x8F
        XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX,
        // 0x90-0x9F
        XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX,
        // 0xA0-0xAF
        XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX,
        // 0xB0-0xBF
        XX, XX, S1, S1, S1, S1, S1, S1, S1, S1, S1, S1, S1, S1, S1, S1,
        // 0xC0-0xCF
        S1, S1, S1, S1, S1, S1, S1, S1, S1, S1, S1, S1, S1, S1, S1, S1,
        // 0xD0-0xDF
        S2, S3, S3, S3, S3, S3, S3, S3, S3, S3, S3, S3, S3, S4, S3, S3,
        // 0xE0-0xEF
        S5, S6, S6, S6, S7, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX]

    struct AcceptRange
    {
        lo:     u8
        hi:     u8
    }

    const AcceptRanges: [?] AcceptRange = [
        {LoCB, HiCB},
        {0xA0, HiCB},
        {LoCB, 0x9F},
        {0x90, HiCB},
        {LoCB, 0x8F}]
}

// Get the last unicode rune of the utf8 slice, and the number of
// bytes to encode it
func decodeLastRune(buffer: const [..] u8)->{ c: rune, eat: u32 }
{
    var result: retval

    let countof = @countof(buffer)
    if !countof
    {
        result.c = RuneError
        return result
    }

    let dataof = @dataof(buffer)
    var ptr    = dataof + (countof - 1)

    // Ascii
    var c = cast(rune) dref ptr
    if c & 0x80 == 0
    {
        result.c   = c
        result.eat = 1
        return result
    }

    var eat = 1'u32
    for
    {
        c = dref ptr
        if c >> 6 != 2 do
            break
        if ptr == dataof
        {
            result.c   = RuneError
            result.eat = 1
            return result
        }

        ptr -= 1
        eat += 1
        if eat > 4
        {
            result.c   = RuneError
            result.eat = 1
            return result
        }
    }

    result = decodeRune(@mkslice(ptr, cast(u64) eat))
    if result.eat != eat
    {
        result.c   = RuneError
        result.eat = 1
    }

    return result
}

// Get the unicode rune pointed by 'buffer', and the number of bytes to encode it
//
// Will return 'RuneError' for an invalid utf8 sequence
func decodeRune(buffer: const [..] u8)->{ c: rune, eat: u32 }
{
    var result: retval

    let countof = @countof(buffer)
    if countof == 0
    {
        result.c = RuneError
        return result
    }

    let dataof = @dataof(buffer)
    let p0     = cast(rune) dref dataof
    if (p0 & 0x80) == 0
    {
        result.c   = p0
        result.eat = 1
        return result
    }

    let x = First[cast(u8) p0]

    // Invalid first byte
    if x == XX
    {
        result.c   = RuneError
        result.eat = 1
        return result
    }

    // Be sure input buffer is large enough to hold the requested
    // encoded size
    let sz = x & 7
    if countof < sz
    {
        result.c   = RuneError
        result.eat = 1
        return result
    }

    let accept = AcceptRanges[x >> 4]

    // Length == 2
    let b1 = dataof[1]
    if !(accept.lo <= b1 and b1 <= accept.hi)
    {
        result.c   = RuneError
        result.eat = 1
        return result
    }

    if sz <= 2
    {
        result.c   = (p0 & Mask2) << 6 | (b1 & MaskX)
        result.eat = 2
        return result
    }

    // Length == 3
    let b2 = dataof[2]
    if !(LoCB <= b2 and b2 <= HiCB)
    {
        result.c   = RuneError
        result.eat = 1
        return result
    }

    if sz <= 3
    {
        result.c   = (p0 & Mask3) << 12 | cast(rune) (b1 & MaskX) << 6 | (b2 & MaskX)
        result.eat = 3
        return result
    }

    // Length == 4
    let b3 = dataof[3]
    if !(LoCB <= b3 and b3 <= HiCB)
    {
        result.c   = RuneError
        result.eat = 1
        return result
    }

    result.c   = (p0 & Mask4) << 18 | cast(rune) (b1 & MaskX) << 12 | cast(rune) (b2 & MaskX) << 6 | (b3 & MaskX)
    result.eat = 4
    return result
}

// Convert rune 'src' to an utf8 sequence, and returns
// the number of bytes that were needed to make the conversion.
//
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
        dest[0] = cast(u8) (((src >> 6) & 0x1F) | 0xC0)
        dest[1] = cast(u8) (((src >> 0) & 0x3F) | 0x80)
        return 2

    case !isValidRune(src):
        dest[0] = cast(u8) (((RuneError >> 12) & 0x0F) | 0xE0)
        dest[1] = cast(u8) (((RuneError >> 6) & 0x3F) | 0x80)
        dest[2] = cast(u8) (((RuneError >> 0) & 0x3F) | 0x80)
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

// Convert a rune array to an utf8 buffer
//
// Returns the number of bytes written in the destination buffer
// 'dest' must be at least 4 bytes long
#[Swag.Overload]
func fromUnicode(dest: [..] u8, src: const [..] rune)->u64
{
    let dataof = @dataof(dest)
    var remain = @countof(dest)

    var total = 0'u64
    foreach ch in src
    {
        Debug.assert(remain >= 4, "destination buffer is too small")
        let sl  = @mkslice(dataof + total, remain)
        let cpt = encodeRune(sl, ch)
        total += cpt
        remain -= cpt
    }

    return total
}

// Convert an unicode buffer to a String
#[Swag.Overload]
func fromUnicode(src: const [..] rune)->String
{
    var result: retval
    result.reserve(4 * @countof(src))
    result.length = fromUnicode(@mkslice(result.buffer, result.capacity), src)
    return result
}

// Convert an utf16 array to an utf8 buffer
//
// Returns the number of bytes written in the destination buffer.
// 'dest' must be at least 4 bytes long
#[Swag.Overload]
func fromUtf16(dest: [..] u8, src: const [..] u16)->u64
{
    var count      = 0'u64
    var srcPtr     = @dataof(src)
    var destPtr    = @dataof(dest)
    var srcLength  = @countof(src)
    var destLength = @countof(dest)

    while srcLength
    {
        let (c, eat) = Utf16.decodeRune(@mkslice(srcPtr, srcLength))

        Debug.assert(destLength >= 4, "destination buffer is too small")
        let eatDest = encodeRune(@mkslice(destPtr, destLength), c)
        destPtr, count += eatDest
        destLength -= eatDest

        srcPtr += eat
        srcLength -= eat
    }

    return count
}

// Convert an utf16 buffer to a String
#[Swag.Overload]
func fromUtf16(src: const [..] u16)->String
{
    var result: retval
    result.reserve(4 * @countof(src))
    result.length = fromUtf16(@mkslice(result.buffer, result.capacity), src)
    Debug.assert(result.length + 1 < result.capacity)
    result.buffer[result.length] = 0
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

// Returns the number of bytes to encode the first rune of the utf8 buffer
//
// If it's an invalid encoding, returns 1.
func countBytesAt(buffer: const [..] u8)->u32
{
    let dataof  = @dataof(buffer)
    let countof = @countof(buffer)
    if !countof do
        return 1

    let p0 = cast(rune) dref dataof
    if (p0 & 0x80) == 0 do
        return 1

    // Invalid first byte
    let x = First[cast(u8) p0]
    if x == XX do
        return 1

    // Be sure input buffer is large enough to hold the requested
    // encoded size
    let sz = x & 7
    if countof < sz do
        return 1

    let accept = AcceptRanges[x >> 4]

    let b1 = dataof[1]
    if !(accept.lo <= b1 and b1 <= accept.hi) do
        return 1
    if sz <= 2 do
        return 2

    let b2 = dataof[2]
    if !(LoCB <= b2 and b2 <= HiCB) do
        return 1
    if sz <= 3 do
        return 3

    let b3 = dataof[3]
    if !(LoCB <= b3 and b3 <= HiCB) do
        return 1
    return 4
}

// Returns the byte index of the given rune index
func byteIndex(buffer: const [..] u8, charIndex: u64)->{ index: u64, success: bool }
{
    var result: retval

    var countof = @countof(buffer)
    if !countof do
        return result

    if !charIndex
    {
        result.success = true
        return result
    }

    var dataof = @dataof(buffer)

    var resultChar = 0'u64
    var resultByte = 0'u64
    while countof and resultChar != charIndex
    {
        let val = countBytesAt(@mkslice(dataof, countof))
        Debug.assert(val <= countof, "invalid utf8 character")
        resultByte += val
        resultChar += 1
        dataof += val
        countof -= val
    }

    result.index   = resultByte
    result.success = resultByte < @countof(buffer)
    return result
}

// Returns the number of runes in an utf8 buffer
func countRunes(buffer: const [..] u8)->u64
{
    var dataof  = @dataof(buffer)
    var countof = @countof(buffer)

    var cpt = 0'u64
    while countof
    {
        let val = countBytesAt(@mkslice(dataof, countof))
        Debug.assert(val <= countof)
        cpt += 1
        dataof += val
        countof -= val
    }

    return cpt
}

// Find the first occurence of rune 'what', and returns the byte index of it.
// Returns 'Swag.U64.Max' if not found
#[Swag.Overload]
func indexOf(src: const [..] u8, what: rune, startByteIndex = 0'u64)->u64
{
    return indexOfAny(src, [what], startByteIndex)
}

// Find the given string, and returns the byte index of it
#[Swag.Overload]
func indexOf(src: const [..] u8, what: string, startByteIndex = 0'u64, comparisonType = ComparisonType.Latin1)->u64
{
    let length = @countof(src)
    if !length do
        return Swag.U64.Max
    Debug.safetyBoundCheck(startByteIndex < length, "index out of range")

    let countof = @countof(what)
    if !countof or countof > (length - startByteIndex) do
        return Swag.U64.Max

    var idx    = startByteIndex
    var scan   = @dataof(src) + startByteIndex
    var remain = length - startByteIndex
    while countof <= remain
    {
        let res = compare(@mkslice(scan, countof), what, comparisonType)
        if res == 0 do
            return idx
        scan, idx += 1
        remain -= 1
    }

    return Swag.U64.Max
}

// Find one of the runes in 'what', and returns the byte index of it.
// Returns 'Swag.U64.Max' if not found
#[Swag.Overload]
func indexOfAny(src: const [..] u8, what: const [..] u8, startByteIndex = 0'u64)->u64
{
    let length = @countof(src)
    if !length do
        return Swag.U64.Max
    Debug.safetyBoundCheck(startByteIndex < length, "index out of range")

    let countof = @countof(what)
    if !countof do
        return Swag.U64.Max

    var idx    = startByteIndex
    var scan   = @dataof(src) + startByteIndex
    var remain = length - startByteIndex
    while remain
    {
        if Slice.contains(what, dref scan) do
            return idx
        scan, idx += 1
        remain -= 1
    }

    return Swag.U64.Max
}

// Find one of the runes in 'what', and returns the byte index of it
#[Swag.Overload]
func indexOfAny(src: const [..] u8, what: const [..] rune, startByteIndex = 0'u64)->u64
{
    let length = @countof(src)
    if !length do
        return Swag.U64.Max
    Debug.safetyBoundCheck(startByteIndex < length, "index out of range")

    let countof = @countof(what)
    if !countof do
        return Swag.U64.Max

    var idx    = startByteIndex
    var scan   = @dataof(src) + startByteIndex
    var remain = length - startByteIndex
    while remain
    {
        let res = Utf8.decodeRune(@mkslice(scan, remain))
        if !res.eat do
            break
        if Slice.contains(what, res.c) do
            return idx
        scan, idx += res.eat
        remain -= res.eat
    }

    return Swag.U64.Max
}

// Returns true if 'src' contains the string 'what'
#[Swag.Overload]
func contains(src: const [..] u8, what: string)->bool
{
    return indexOf(src, what) != Swag.U64.Max
}

// Returns true if 'src' contains the string 'what'
#[Swag.Overload]
func contains(src: const [..] u8, what: rune)->bool
{
    return indexOf(src, what) != Swag.U64.Max
}

// Returns true if 'src' contains the string 'what'
#[Swag.Overload]
func contains(src: const [..] u8, what: u8)->bool
{
    return Slice.contains(src, what)
}

// Returns true if 'src' contains the slice 'what'
#[Swag.Overload]
func contains(src, what: const [..] u8)->bool
{
    return Slice.findSlice(src, what) != Swag.U32.Max
}

// Find the last rune occurence of 'what', and returns the byte index of it.
// Returns 'Swag.U64.Max' if not found
#[Swag.Overload]
func lastIndexOf(src: const [..] u8, what: rune)->u64
{
    return lastIndexOfAny(src, [what])
}

// Returns the last index (in bytes) of a any of the runes in 'what'.
// Returns 'Swag.U64.Max' if not found
#[Swag.Overload]
func lastIndexOfAny(src: const [..] u8, what: const [..] rune)->u64
{
    let length = @countof(src)
    if !length do
        return Swag.U64.Max
    let dataof = @dataof(src)

    var byteIdx = length
    for
    {
        let sl = @mkslice(dataof, byteIdx)
        let (c, eat) = decodeLastRune(sl)
        if eat == 0 do
            return Swag.U64.Max

        byteIdx -= eat
        if Slice.contains(what, c) do
            return byteIdx
    }

    return Swag.U64.Max
}

// Returns the last index (in bytes) of a any of the bytes in 'what'.
// Returns 'Swag.U64.Max' if not found
#[Swag.Overload]
func lastIndexOfAny(src: const [..] u8, what: const [..] u8)->u64
{
    let length = @countof(src)
    if !length do
        return Swag.U64.Max

    var scan    = @dataof(src) + length
    var byteIdx = length
    for
    {
        scan, byteIdx -= 1
        if Slice.contains(what, dref scan) do
            return byteIdx
    }

    return Swag.U64.Max
}

// Returns the last index (in bytes) of a string.
// Returns 'Swag.U64.Max' if not found
#[Swag.Overload]
func lastIndexOf(src: const [..] u8, what: string, comparisonType = ComparisonType.Latin1)->u64
{
    let length = @countof(src)
    if !length do
        return Swag.U64.Max
    let lengthStr = @countof(what)
    if lengthStr > length do
        return Swag.U64.Max

    var idx    = length - lengthStr
    var dataof = @dataof(src) + idx
    for
    {
        let res = compare(@mkslice(dataof, lengthStr), what, comparisonType)
        if res == 0 do
            return idx
        if !idx do
            return Swag.U64.Max
        idx, dataof -= 1
    }

    return Swag.U64.Max
}

// Return true if the string starts with 'str'
func startsWith(src: #null const [..] u8, str: string, comparisonType = ComparisonType.Latin1)->bool
{
    let length = @countof(src)
    if !length do
        return false
    let countof = @countof(str)
    if countof > length do
        return false
    let buffer = @dataof(src)
    return compare(@mkslice(buffer, countof), str, comparisonType) == 0
}

// Return true if the string ends with 'str'
func endsWith(src: #null const [..] u8, str: string, comparisonType = ComparisonType.Latin1)->bool
{
    let length = @countof(src)
    if !length do
        return false
    let countof = @countof(str)
    if countof > length do
        return false
    let buffer = @dataof(src)
    return compare(@mkslice((buffer + length) - countof, countof), str, comparisonType) == 0
}

// Returns the first rune of the slice
func firstRune(src: #null const [..] u8)->rune
{
    let countof = @countof(src)
    if !countof do
        return 0
    let dataof = @dataof(src)
    if Latin1.isAscii(dataof[0]) do
        return cast(rune) dataof[0]
    let (c, ?) = Utf8.decodeRune(src)
    return c
}

// Returns the last rune of the slice
func lastRune(src: #null const [..] u8)->rune
{
    let countof = @countof(src)
    if !countof do
        return 0
    let dataof = @dataof(src)
    if Latin1.isAscii(dataof[countof - 1]) do
        return cast(rune) dataof[countof - 1]
    let (c, ?) = Utf8.decodeLastRune(src)
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

    #[Swag.Complete]
    switch comparisonType
    {
    case Latin1:
        return Core.Latin1.compare(src, dst) // bytes compare

    case Latin1NoCase:
        return Core.Latin1.compare(src, dst, ignoreCase: true)

    case Unicode:
        return Core.Latin1.compare(src, dst) // bytes compare

    case UnicodeNoCase:
        var length      = @countof(src)
        var otherLength = @countof(dst)
        var data        = @dataof(src)
        var otherData   = @dataof(dst)

        var result: s32
        var c1, c2: rune
        var eat1, eat2: u32
        for
        {
            (c1, eat1) = decodeRune(@mkslice(data, length))
            (c2, eat2) = decodeRune(@mkslice(otherData, otherLength))
            if eat1 == 0 or eat2 == 0 do
                break

            if c1 != c2
            {
                c1     = Core.Unicode.toLower(c1)
                c2     = Core.Unicode.toLower(c2)
                result = cast(s32) c1 - cast(s32) c2
                if result != 0 do
                    return Math.sign(result)
            }

            length -= eat1
            otherLength -= eat2
            data += eat1
            otherData += eat2
        }

        if !eat1 and eat2 do
            return -1
        if eat1 and !eat2 do
            return 1
        return 0
    }

    return 0
}

// Macro to foreach the unicode characters of the utf8 sequence.
// * '#alias0' will contain the character
// * '#alias1' will contain the character index
// * '#alias2' will contain the byte index of that character within the utf8 sequence
#[Swag.Macro]
func visitRunes(buffer: const [..] u8, stmt: #code void)
{
    var count = @countof(buffer)
    if !count do
        return

    var byteIdx = 0'u64
    var charIdx = 0'u64
    var scan    = @dataof(buffer)
    while count
    {
        let sl = @mkslice(scan, count)
        let (c, eat) = decodeRune(sl)
        if eat == 0 do
            break

        #macro
        {
            #scope(Inside)
            {
                let #alias0 = #up c
                let #alias1 = #up charIdx
                let #alias2 = #up byteIdx
                #inject(#up stmt, continue = break to Inside)
            }
        }

        charIdx += 1
        byteIdx, scan += eat
        count -= eat
    }
}

// Returns true if the utf8 sequence is valid
func isValid(buffer: const [..] u8)->bool
{
    var p       = @dataof(buffer)
    var countof = @countof(buffer)
    if !countof do
        return true

    // Quick zap ascii characters
    while countof >= 8
    {
        let first64 = dref cast(const *u64) p
        if first64 & 0x80808080_80808080 do
            break
        p += 8
        countof -= 8
    }

    while countof
    {
        let p0 = cast(rune) dref p
        if (p0 & 0x80) == 0
        {
            p += 1
            countof -= 1
            continue
        }

        // Invalid first byte
        let x = First[cast(u8) p0]
        if x == XX do
            return false

        // Be sure input buffer is large enough to hold the requested
        // encoded size
        let sz = x & 7
        if countof < sz do
            return false

        let accept = AcceptRanges[x >> 4]

        let b1 = p[1]
        if !(accept.lo <= b1 and b1 <= accept.hi) do
            return false
        if sz <= 2
        {
            p += 2
            countof -= 2
            continue
        }

        let b2 = p[2]
        if !(LoCB <= b2 and b2 <= HiCB) do
            return false
        if sz <= 3
        {
            p += 3
            countof -= 3
            continue
        }

        let b3 = p[3]
        if !(LoCB <= b3 and b3 <= HiCB) do
            return false
        p += 4
        countof -= 4
    }

    return true
}

func beautifyName(name: const [..] u8)->String
{
    var result: retval

    var first    = true
    var wasUpper = true
    visitRunes(|c| name)
    {
        if Unicode.isUpper(c)
        {
            if !wasUpper
            {
                wasUpper = true
                result += " "
            }
        }
        else
        {
            wasUpper = false
        }

        if first
        {
            result += Unicode.toUpper(c)
            first = false
        }
        else do
            result += c
    }

    return result
}
