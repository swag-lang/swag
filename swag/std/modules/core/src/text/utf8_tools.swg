#global public
#global namespace utf8

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
        res := utf8.decodeChar(@mkslice(scan, remain))
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
        res := compare(@mkslice(scan, countof), what, comparisonType)
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
        (c, eat) := decodeLastChar(slice)
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
    return compare(@mkslice(buffer, countof), str, comparisonType) == 0
}

// Return true if the string ends with 'str'
func endsWith(src: const [..] u8, str: string, comparisonType = utf8.ComparisonType.Latin1)->bool
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
            slice := @mkslice(ptr, count)
            (c, eat) = utf8.decodeChar(slice)
            if eat == 0 || !contains(separators, c)
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
            slice := @mkslice(ptr, count)
            (c, eat) = utf8.decodeChar(slice)
            if eat == 0 || contains(separators, c)
                break
            endbyteIdx += eat
            ptr += eat
            count -= eat
        }

        oneString := cast(string) @mkslice(dataof + startbyteIdx, endbyteIdx - startbyteIdx)
        result.add(oneString)

        if eat == 0 break // done
        startbyteIdx = endbyteIdx
    }

    return result
}
