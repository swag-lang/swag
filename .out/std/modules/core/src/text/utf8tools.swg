#global public
#global namespace utf8

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

	accept := AcceptRanges[x >>% 4]

	b1 := dataof[1]
	if !(accept.lo <= b1 && b1 <= accept.hi)
        return 1
	if sz <= 2
        return 2

	b2 := dataof[2]
    if !(LoCB <= b2 && b2 <= HiCB)
        return 1
	if sz <= 3
        return 3

	b3 := dataof[3]
	if !(LoCB <= b3 && b3 <= HiCB)
        return 1
	return 4
}

// Returns the byte index of the given character index
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
    while countof && resultChar != charIndex
    {
        val := countBytesAt(@mkslice(dataof, countof))
        debug.assert(val <= countof, "invalid utf8 character")
        resultByte += val
        resultChar += 1
        dataof += val
        countof -= val
    }

    result.index = resultByte
    result.success = resultByte < @countof(buffer)
    return result
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

// Find the first occurence of character 'what', and returns the byte index of it
// Returns swag.UInt.Max if not found
func indexOf(src: const [..] u8, what: char, startByteIndex = 0'uint)->uint
{
    return indexOf(src, @[what], startByteIndex)
}

// Find one of the characters in 'what', and returns the byte index of it
// Returns swag.UInt.Max if not found
func indexOf(src: const [..] u8, what: const [..] char, startByteIndex = 0'uint)->uint
{
    length := @countof(src)
    if !length
        return swag.UInt.Max
    debug.assert(startByteIndex < length, "invalid 'startByteIndex'")

    countof := @countof(what)
    if !countof
        return swag.UInt.Max

    idx := startByteIndex
    scan := @dataof(src) + startByteIndex
    remain := length - startByteIndex
    while countof <= remain
    {
        res := utf8.decodeChar(@mkslice(scan, remain))
        if !res.eat
            break
        if slice.contains(what, res.c)
            return idx
        scan, idx += res.eat
        remain -= res.eat
    }

    return swag.UInt.Max
}

// Find the given string, and returns the byte index of it
// Returns swag.UInt.Max if not found
func indexOf(src: const [..] u8, what: string, startByteIndex = 0'uint, comparisonType = ComparisonType.Latin1)->uint
{
    length := @countof(src)
    if !length
        return swag.UInt.Max
    debug.assert(startByteIndex < length, "invalid 'startByteIndex'")

    countof := @countof(what)
    if !countof || countof > (length - startByteIndex)
        return swag.UInt.Max

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

    return swag.UInt.Max
}

// Find the last character occurence of 'what', and returns the byte index of it
// Returns swag.UInt.Max if not found
func lastIndexOf(src: const [..] u8, what: char)->uint
{
    return lastIndexOf(src, @[what])
}

// Returns the last index (in bytes) of a character
// Returns swag.UInt.Max if not found
func lastIndexOf(src: const [..] u8, what: const [..] char)->uint
{
    length := @countof(src)
    if !length
        return swag.UInt.Max
    dataof := @dataof(src)

    byteIdx := length
    while true
    {
        sl := @mkslice(dataof, byteIdx)
        (c, eat) := decodeLastChar(sl)
        if eat == 0
            return swag.UInt.Max

        byteIdx -= eat
        if slice.contains(what, c)
            return byteIdx
    }

    return swag.UInt.Max
}

// Returns the last index (in bytes) of a string
// Returns swag.UInt.Max if not found
func lastIndexOf(src: const [..] u8, what: string, comparisonType = ComparisonType.Latin1)->uint
{
    length := @countof(src)
    if !length
        return swag.UInt.Max
    lengthStr := @countof(what)
    if lengthStr > length
        return swag.UInt.Max

    idx := length - lengthStr
    dataof := @dataof(src) + idx
    while true
    {
        res := compare(@mkslice(dataof, lengthStr), what, comparisonType)
        if res == 0
            return idx
        if !idx
            return swag.UInt.Max
        idx, dataof -= 1
    }

    return swag.UInt.Max
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

// Returns the first unicode character
func firstChar(src: const [..] u8)->char
{
    countof := @countof(src)
    if !countof
        return 0
    dataof := @dataof(src)
    if latin1.isAscii(dataof[0])
        return cast(char) dataof[0]
    (c, ?) := utf8.decodeChar(src)
    return c
}

// Returns the last unicode character
func lastChar(src: const [..] u8)->char
{
    countof := @countof(src)
    if !countof
        return 0
    dataof := @dataof(src)
    if latin1.isAscii(dataof[countof - 1])
        return cast(char) dataof[countof - 1]
    (c, ?) := utf8.decodeLastChar(src)
    return c
}

// Split string into sub strings, given a character separator
// Note that this returns an array of native strings, not copies.
// All strings will be invalid if 'src' is destroyed
func split(src: const [..] u8, separator: char) -> Array'string
{
    return split(src, @[separator])
}

// Split string into sub strings, given an array of character separators
// Note that this returns an array of native strings, not copies.
// All strings will be invalid if 'src' String is destroyed
func split(src: const [..] u8, separators: const [..] char) -> Array'string
{
    dataof := @dataof(src)
    countof := @countof(src)

    var result: retval
    var ptr = dataof
    var c: char
    var eat: uint

    startbyteIdx := 0'uint
    count := countof
    while true
    {
        // Remove separator at the start
        while true
        {
            sl := @mkslice(ptr, count)
            (c, eat) = utf8.decodeChar(sl)
            if eat == 0 || !slice.contains(separators, c)
                break
            startbyteIdx += eat
            ptr += eat
            count -= eat
        }

        if eat == 0 break // done

        // Add all real characters until we found a separator
        endbyteIdx := startbyteIdx
        while true
        {
            sl := @mkslice(ptr, count)
            (c, eat) = utf8.decodeChar(sl)
            if eat == 0 || slice.contains(separators, c)
                break
            endbyteIdx += eat
            ptr += eat
            count -= eat
        }

        oneString := @mkstring(dataof + startbyteIdx, endbyteIdx - startbyteIdx)
        result.add(oneString)

        if eat == 0 break // done
        startbyteIdx = endbyteIdx
    }

    return result
}

// Returns a sub string starting at 'startByteIndex' and ending with 'delimiter'
func eatTo(src: const [..] u8, delimiter: u8, startByteIndex = 0'uint, includeDelimiter = true)->const [..] u8
{
    dataof := @dataof(src)
    length := @countof(src)
    if !length
        return @mkslice(dataof, 0)
    debug.assert(startByteIndex < length, "invalid 'startByteIndex'")

    length -= startByteIndex
    dataof += startByteIndex
    scan := dataof
    while length
    {
        if :scan == delimiter
        {
            if includeDelimiter
                scan += 1
            return @mkslice(dataof, cast(uint) (scan - dataof))
        }

        scan += 1
        length -= 1
    }

    return @mkslice(dataof, 0)
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
            (c1, eat1) = decodeChar(@mkslice(data, length))
            (c2, eat2) = decodeChar(@mkslice(otherData, otherLength))
            if eat1 == 0 || eat2 == 0
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
// @alias1 will contain the character index
// @alias2 will contain the byte index of that character within the utf8 sequence
#[swag.macro]
func(ptr: bool) opVisitChars(buffer: const [..] u8, stmt: code)
{
    #assert(!ptr, "visiting by pointer not allowed")
    count := @countof(buffer)
    if !count
        return

    byteIdx := 0'uint
    charIdx := 0'uint
    scan := @dataof(buffer)
    while count
    {
        sl := @mkslice(scan, count)
        (c, eat) := decodeChar(sl)
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

        accept := AcceptRanges[x >>% 4]

        b1 := p[1]
        if !(accept.lo <= b1 && b1 <= accept.hi)
            return false
        if sz <= 2
        {
            p += 2
            countof -= 2
            continue
        }

        b2 := p[2]
        if !(LoCB <= b2 && b2 <= HiCB)
            return false
        if sz <= 3
        {
            p += 3
            countof -= 3
            continue
        }

        b3 := p[3]
        if !(LoCB <= b3 && b3 <= HiCB)
            return false
        p += 4
        countof -= 4
    }

    return true
}
