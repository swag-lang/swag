#global public
#global namespace Tokenize

// Remove ""
func eatQuotes(str: string)->string
{
    let countof = @countof(str)
    if countof < 2:
        return str
    if str[0] == `"` and str[countof - 1] == `"`:
        return @mkstring(@dataof(str) + 1, countof - 2)
    return str
}

// Eat 'count' bytes, and returns the remaining string
func eatCount(str: string, count: u64)->string
{
    let countof = @countof(str)
    let cpt     = Math.min(count, countof)
    return @mkstring(@dataof(str) + cpt, countof - cpt)
}

// Eat all spaces, and returns the remaining string (trim left)
func eatSpaces(str: string)->string
{
    let countof = @countof(str)
    if !countof:
        return str

    var cpt = 0'u64
    while cpt < countof and Latin1.isSpace(str[cpt]):
        cpt += 1
    return @mkstring(@dataof(str) + cpt, countof - cpt)
}

// Split the string into an array of lines
// Note that this returns an array of native strings, not copies.
// All strings will be invalid if 'src' is destroyed
func splitLines(src: string)->Array'string
{
    var result: retval
    let countof = @countof(src)
    if !countof:
        return result

    var dataof = @dataof(src)
    var cpt    = 0
    var end    = dataof
    while cpt < countof
    {
        while cpt < countof and dref end != `\r` and dref end != `\n`
        {
            end += 1
            cpt += 1
        }

        let onestr = @mkstring(dataof, cast(u64) (end - dataof))
        result.add(onestr)
        if cpt < countof - 1 and dref end == `\r` and end[1] == `\n`:
            end, cpt += 1
        end, cpt += 1
        dataof = end
    }

    return result
}

// Split string into sub strings, given a byte separator
// Note that this returns an array of native strings, not copies.
// All strings will be invalid if 'src' String is destroyed
#[Swag.Overload]
func split(src: string, separator: u8, maxSplit: u32 = 0, removeEmpty = true)->Array'string
{
    let countof = @countof(src)

    var result:       retval
    var startbyteIdx: u64

    while !maxSplit or result.count != maxSplit
    {
        // Remove separator at the start
        while startbyteIdx != countof
        {
            if src[startbyteIdx] != separator:
                break
            startbyteIdx += 1
            if !removeEmpty:
                break
        }

        if startbyteIdx == countof:
            break

        // Add all real characters until we found a separator
        var endbyteIdx = startbyteIdx
        while endbyteIdx != countof
        {
            if src[endbyteIdx] == separator:
                break
            endbyteIdx += 1
        }

        result.add(src[startbyteIdx..<endbyteIdx])
        startbyteIdx = endbyteIdx

        if startbyteIdx == countof:
            break

        startbyteIdx += 1
    }

    // Last split
    if startbyteIdx != countof:
        result.add(src[startbyteIdx..<countof])

    return result
}

// Split string into sub strings, given a string separator
// Note that this returns an array of native strings, not copies.
// All strings will be invalid if 'src' String is destroyed
#[Swag.Overload]
func split(src: string, separator: string, maxSplit: u32 = 0, removeEmpty = true)->Array'string
{
    let countof = @countof(src)

    var result:       retval
    var startbyteIdx: u64

    while !maxSplit or result.count != maxSplit
    {
        // Remove separator at the start
        while startbyteIdx != countof
        {
            if !Utf8.startsWith(src[startbyteIdx..<countof], separator):
                break
            startbyteIdx += @countof(separator)
            if !removeEmpty:
                break
        }

        if startbyteIdx == countof:
            break

        // Add all real characters until we found a separator
        var endbyteIdx = startbyteIdx
        while endbyteIdx != countof
        {
            if Utf8.startsWith(src[endbyteIdx..<countof], separator):
                break
            endbyteIdx += 1
        }

        if startbyteIdx == endbyteIdx:
            result.add("")
        else:
            result.add(src[startbyteIdx..<endbyteIdx])

        startbyteIdx = endbyteIdx
        if startbyteIdx == countof:
            break
    }

    // Last split
    if startbyteIdx != countof
    {
        startbyteIdx += @countof(separator)
        result.add(src[startbyteIdx..<countof])
    }

    return result
}

// Split string into sub strings, given a rune separator
// Note that this returns an array of native strings, not copies.
// All strings will be invalid if 'src' is destroyed
#[Swag.Overload]
func split(src: string, separator: rune, maxSplit: u32 = 0, removeEmpty = true)->Array'string
{
    return splitAny(src, [separator], maxSplit, removeEmpty)
}

// Split string into sub strings, given an array of rune separators
// Note that this returns an array of native strings, not copies.
// All strings will be invalid if 'src' String is destroyed
func splitAny(src: string, separators: const [..] rune, maxSplit: u32 = 0, removeEmpty = true)->Array'string
{
    let countof = @countof(src)

    var result: retval
    var c:      rune
    var startbyteIdx, eat: u64

    while !maxSplit or result.count != maxSplit
    {
        // Remove separator at the start
        while startbyteIdx != countof
        {
            (c, eat) = Utf8.decodeRune(src[startbyteIdx..<countof])
            if !Slice.contains(separators, c):
                break
            startbyteIdx += eat
            if !removeEmpty:
                break
        }

        if startbyteIdx == countof:
            break

        // Add all real characters until we found a separator
        var endbyteIdx = startbyteIdx
        while endbyteIdx != countof
        {
            (c, eat) = Utf8.decodeRune(src[endbyteIdx..<countof])
            if Slice.contains(separators, c):
                break
            endbyteIdx += eat
        }

        if startbyteIdx == endbyteIdx:
            result.add("")
        else:
            result.add(src[startbyteIdx..<endbyteIdx])

        startbyteIdx = endbyteIdx
        if startbyteIdx == countof:
            break
    }

    // Last split
    if startbyteIdx != countof
    {
        startbyteIdx += eat
        result.add(src[startbyteIdx..<countof])
    }

    return result
}

// Returns the first substring of 'str' stopping at the first blank character
func getToSpace(str: string)->string
{
    let countof = @countof(str)
    var cpt     = 0'u64
    while cpt < countof and !Latin1.isSpace(str[cpt]):
        cpt += 1
    return @mkstring(@dataof(str), cpt)
}

// Returns the first substring of 'str' containing only digits and letters
func getWhileAlnum(str: string)->string
{
    let countof = @countof(str)
    var cpt     = 0'u64
    while cpt < countof and (Latin1.isDigit(str[cpt]) or Latin1.isLetter(str[cpt])):
        cpt += 1
    return @mkstring(@dataof(str), cpt)
}

// Returns a sub string starting at 'startByteIndex' and ending with 'delimiter'
func getTo(src: string, delimiter: u8, startByteIndex = 0'u64, includeDelimiter = true)->string
{
    var dataof = @dataof(src)
    var length = @countof(src)
    if !length:
        return @mkstring(dataof, 0)
    Debug.assert(startByteIndex < length, "invalid 'startByteIndex'")

    length -= startByteIndex
    dataof += startByteIndex
    var scan = dataof
    while length
    {
        if dref scan == delimiter
        {
            if includeDelimiter:
                scan += 1
            return @mkstring(dataof, cast(u64) (scan - dataof))
        }

        scan += 1
        length -= 1
    }

    return @mkstring(dataof, 0)
}
