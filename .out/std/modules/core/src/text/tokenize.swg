#global public
#global namespace tokenize

// Split string into sub strings, given a rune separator
// Note that this returns an array of native strings, not copies.
// All strings will be invalid if 'src' is destroyed
func split(src: const [..] u8, separator: rune) -> Array'string
{
    return split(src, @[separator])
}

// Split string into sub strings, given an array of rune separators
// Note that this returns an array of native strings, not copies.
// All strings will be invalid if 'src' String is destroyed
func split(src: const [..] u8, separators: const [..] rune) -> Array'string
{
    dataof := @dataof(src)
    countof := @countof(src)

    var result: retval
    var ptr = dataof
    var c: rune
    var eat: uint

    startbyteIdx := 0'uint
    count := countof
    while true
    {
        // Remove separator at the start
        while true
        {
            sl := @mkslice(ptr, count)
            (c, eat) = utf8.decodeRune(sl)
            if eat == 0 or !slice.contains(separators, c)
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
            (c, eat) = utf8.decodeRune(sl)
            if eat == 0 or slice.contains(separators, c)
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
