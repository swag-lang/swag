#global public
#global namespace Tokenize

// Remove ""
func eatQuotes(str: string)->string
{
    let countof = @countof(str)
    if countof < 2 return str
    if str[0] == `"` and str[countof - 1] == `"`
        return @mkstring(@dataof(str) + 1, countof - 2)
    return str
}

// Eat 'count' bytes, and returns the remaining string
func eatCount(str: string, count: u64)->string
{
    let countof = @countof(str)
    let cpt = Math.min(count, countof)
    return @mkstring(@dataof(str) + cpt, countof - cpt)
}

// Eat all spaces, and returns the remaining string (trim left)
func eatSpaces(str: string)->string
{
    let countof = @countof(str)
    if !countof return str

    var cpt = 0'u64
    while cpt < countof and Latin1.isSpace(str[cpt])
        cpt += 1
    return @mkstring(@dataof(str) + cpt, countof - cpt)
}

// Split the string into an array of lines
// Note that this returns an array of native strings, not copies.
// All strings will be invalid if 'src' is destroyed
func splitLines(src: string) -> Array'string
{
    var result: retval
    let countof = @countof(src)
    if !countof
        return result

    var dataof = @dataof(src)
    var cpt = 0
    var end = dataof
    while cpt < countof
    {
        while cpt < countof and dref end != `\r` and dref end != `\n`
        {
            end += 1
            cpt += 1
        }

        let onestr = @mkstring(dataof, cast(u64) (end - dataof))
        result.add(onestr)
        if cpt < countof - 1 and dref end == `\r` and end[1] == `\n`
            end, cpt += 1
        end, cpt += 1
        dataof = end
    }

    return result
}

// Split string into sub strings, given a rune separator
// Note that this returns an array of native strings, not copies.
// All strings will be invalid if 'src' is destroyed
func split(src: string, separator: rune, maxSplit: u32 = 0, removeEmpty = true) -> Array'string
{
    return split(src, [separator], maxSplit, removeEmpty)
}

// Split string into sub strings, given an array of rune separators
// Note that this returns an array of native strings, not copies.
// All strings will be invalid if 'src' String is destroyed
func split(src: string, separators: const [..] rune, maxSplit: u32 = 0, removeEmpty = true)->Array'string
{
    let dataof = @dataof(src)
    let countof = @countof(src)

    var result: retval
    var ptr = dataof
    var c: rune
    var eat: u64

    var startbyteIdx = 0'u64
    var count = countof
    var cpt = 0
    loop
    {
        // Remove separator at the start
        loop
        {
            let sl = @mkslice(ptr, count)
            (c, eat) = Utf8.decodeRune(sl)
            if eat == 0 or !Slice.contains(separators, c)
                break
            startbyteIdx += eat
            ptr += eat
            count -= eat
            if !removeEmpty
                break
        }

        if eat == 0
            break // done

        // Add all real characters until we found a separator
        var endbyteIdx = startbyteIdx
        loop
        {
            let sl = @mkslice(ptr, count)
            (c, eat) = Utf8.decodeRune(sl)
            if eat == 0 or Slice.contains(separators, c)
                break
            endbyteIdx += eat
            ptr += eat
            count -= eat
        }

        var oneString = @mkstring(dataof + startbyteIdx, endbyteIdx - startbyteIdx)
        result.add(oneString)

        if eat == 0
            break // done
        startbyteIdx = endbyteIdx

        // Have we reached the maximum number of split ?
        cpt += 1
        if cpt == maxSplit
        {
            startbyteIdx += eat
            oneString = @mkstring(dataof + startbyteIdx, countof - startbyteIdx)
            result.add(oneString)
            break
        }

    }

    return result
}

// Returns the first substring of 'str', stopping at the first blank character
func getToSpace(str: string)->string
{
    let countof = @countof(str)
    var cpt = 0'u64
    while cpt < countof and !Latin1.isSpace(str[cpt])
        cpt += 1
    return @mkstring(@dataof(str), cpt)
}

// Returns the first substring of 'str' containing only digits and letters
func getWhileAlnum(str: string)->string
{
    let countof = @countof(str)
    var cpt = 0'u64
    while cpt < countof and (Latin1.isDigit(str[cpt]) or Latin1.isLetter(str[cpt]))
        cpt += 1
    return @mkstring(@dataof(str), cpt)
}

// Returns a sub string starting at 'startByteIndex' and ending with 'delimiter'
func getTo(src: string, delimiter: u8, startByteIndex = 0'u64, includeDelimiter = true)->string
{
    var dataof = @dataof(src)
    var length = @countof(src)
    if !length
        return @mkstring(dataof, 0)
    Debug.assert(startByteIndex < length, "invalid 'startByteIndex'")

    length -= startByteIndex
    dataof += startByteIndex
    var scan = dataof
    while length
    {
        if dref scan == delimiter
        {
            if includeDelimiter
                scan += 1
            return @mkstring(dataof, cast(u64) (scan - dataof))
        }

        scan += 1
        length -= 1
    }

    return @mkstring(dataof, 0)
}
