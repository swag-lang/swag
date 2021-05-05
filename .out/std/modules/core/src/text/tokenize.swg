#global public
#global namespace tokenize

// Eat 'count' bytes, and returns the remaining string
func eatCount(str: string, count: uint)->string
{
    countof := @countof(str)
    cpt := math.min(count, countof)
    return @mkstring(@dataof(str) + cpt, countof - cpt)
}

// Eat all spaces, and returns the remaining string (trim left)
func eatSpaces(str: string)->string
{
    countof := @countof(str)
    if !countof return str

    cpt := 0'uint
    while cpt < countof and latin1.isSpace(str[cpt])
        cpt += 1
    return @mkstring(@dataof(str) + cpt, countof - cpt)
}

// Split string into sub strings, given a rune separator
// Note that this returns an array of native strings, not copies.
// All strings will be invalid if 'src' is destroyed
func split(src: string, separator: rune) -> Array'string
{
    return split(src, @[separator])
}

// Split string into sub strings, given an array of rune separators
// Note that this returns an array of native strings, not copies.
// All strings will be invalid if 'src' String is destroyed
func split(src: string, separators: const [..] rune) -> Array'string
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

// Returns the first substring of 'str', stopping at the first blank character
func getToSpace(str: string)->string throw
{
    countof := @countof(str)
    if !countof throw "invalid"

    cpt := 0'uint
    while cpt < countof and !latin1.isSpace(str[cpt])
        cpt += 1
    return @mkstring(@dataof(str), cpt)
}

// Returns a sub string starting at 'startByteIndex' and ending with 'delimiter'
func getTo(src: string, delimiter: u8, startByteIndex = 0'uint, includeDelimiter = true)->string
{
    dataof := @dataof(src)
    length := @countof(src)
    if !length
        return @mkstring(dataof, 0)
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
            return @mkstring(dataof, cast(uint) (scan - dataof))
        }

        scan += 1
        length -= 1
    }

    return @mkstring(dataof, 0)
}
