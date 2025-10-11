#global public
#global namespace Tokenize

// Removes surrounding double quotes from the string, if present
func eatQuotes(str: string)->string
{
    let countof = @countof(str)
    if countof < 2 do
        return str
    if str[0] == '"' and str[countof - 1] == '"' do
        return @mkstring(@dataof(str) + 1, countof - 2)
    return str
}

// Eats 'count' bytes from the beginning of the string and returns the remaining string
func eatCount(str: string, count: u64)->string
{
    if str == null do
        return str
    let countof = @countof(str)
    let cpt     = Math.min(count, countof)
    return @mkstring(@dataof(str) + cpt, countof - cpt)
}

// Removes leading space characters from the string and returns the trimmed result
func eatSpaces(str: string)->string
{
    let countof = @countof(str)
    if !countof do
        return str

    var cpt = 0'u64
    while cpt < countof and Latin1.isSpace(str[cpt]) do
        cpt += 1
    return @mkstring(@dataof(str) + cpt, countof - cpt)
}

// Splits the string into an array of lines using newline or carriage return as delimiters
// Note that this returns an array of native strings, not copies.
// All strings will be invalid if 'src' is destroyed
func splitLines(src: string)->Array'string
{
    var result: retval
    let countof = @countof(src)
    if !countof do
        return result

    var dataof = @dataof(src)
    var cpt    = 0
    var end    = dataof
    while cpt < countof
    {
        while cpt < countof and dref end != '\r' and dref end != '\n'
        {
            end += 1
            cpt += 1
        }

        let onestr = @mkstring(dataof, cast(u64) (end - dataof))
        result.add(onestr)
        if cpt < countof - 1 and dref end == '\r' and end[1] == '\n' do
            end, cpt += 1
        end, cpt += 1
        dataof = end
    }

    return result
}

// Splits the string into substrings using a byte as separator
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
            if src[startbyteIdx] != separator do
                break
            startbyteIdx += 1
            if !removeEmpty do
                break
        }

        if startbyteIdx == countof do
            break

        // Add all real characters until we found a separator
        var endbyteIdx = startbyteIdx
        while endbyteIdx != countof
        {
            if src[endbyteIdx] == separator do
                break
            endbyteIdx += 1
        }

        result.add(src[startbyteIdx until endbyteIdx])
        startbyteIdx = endbyteIdx

        if startbyteIdx == countof do
            break

        startbyteIdx += 1
    }

    // Last split
    if startbyteIdx != countof do
        result.add(src[startbyteIdx until countof])

    return result
}

// Splits the string into substrings using another string as separator
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
            if !Utf8.startsWith(src[startbyteIdx until countof], separator) do
                break
            startbyteIdx += @countof(separator)
            if !removeEmpty do
                break
        }

        if startbyteIdx == countof do
            break

        // Add all real characters until we found a separator
        var endbyteIdx = startbyteIdx
        while endbyteIdx != countof
        {
            if Utf8.startsWith(src[endbyteIdx until countof], separator) do
                break
            endbyteIdx += 1
        }

        if startbyteIdx == endbyteIdx do
            result.add("")
        else do
            result.add(src[startbyteIdx until endbyteIdx])

        startbyteIdx = endbyteIdx
        if startbyteIdx == countof do
            break
    }

    // Last split
    if startbyteIdx != countof
    {
        startbyteIdx += @countof(separator)
        result.add(src[startbyteIdx until countof])
    }

    return result
}

// Splits the string into substrings using a rune as separator
// Note that this returns an array of native strings, not copies.
// All strings will be invalid if 'src' is destroyed
#[Swag.Overload]
func split(src: string, separator: rune, maxSplit: u32 = 0, removeEmpty = true)->Array'string
{
    return splitAny(src, [separator], maxSplit, removeEmpty)
}

// Splits the string into substrings using any rune from the given array as separator
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
            (c, eat) = Utf8.decodeRune(src[startbyteIdx until countof])
            if !Slice.contains(separators, c) do
                break
            startbyteIdx += eat
            if !removeEmpty do
                break
        }

        if startbyteIdx == countof do
            break

        // Add all real characters until we found a separator
        var endbyteIdx = startbyteIdx
        while endbyteIdx != countof
        {
            (c, eat) = Utf8.decodeRune(src[endbyteIdx until countof])
            if Slice.contains(separators, c) do
                break
            endbyteIdx += eat
        }

        if startbyteIdx == endbyteIdx do
            result.add("")
        else do
            result.add(src[startbyteIdx until endbyteIdx])

        startbyteIdx = endbyteIdx
        if startbyteIdx == countof do
            break
    }

    // Last split
    if startbyteIdx != countof
    {
        startbyteIdx += eat
        result.add(src[startbyteIdx until countof])
    }

    return result
}

// Returns the first substring of 'str' stopping at the first whitespace character
func getToSpace(str: string)->string
{
    let countof = @countof(str)
    var cpt     = 0'u64
    while cpt < countof and !Latin1.isSpace(str[cpt]) do
        cpt += 1
    return @mkstring(@dataof(str), cpt)
}

// Returns the first substring of 'str' that contains only alphanumeric characters
func getWhileAlnum(str: string)->string
{
    let countof = @countof(str)
    var cpt     = 0'u64
    while cpt < countof and (Latin1.isDigit(str[cpt]) or Latin1.isLetter(str[cpt])) do
        cpt += 1
    return @mkstring(@dataof(str), cpt)
}

// Returns a substring of 'src' starting at 'startByteIndex' and ending at the first occurrence of 'delimiter'
// Includes the delimiter if 'includeDelimiter' is true
func getTo(src: string, delimiter: u8, startByteIndex = 0'u64, includeDelimiter = true)->string
{
    var dataof = @dataof(src)
    var length = @countof(src)
    if !length do
        return @mkstring(dataof, 0)
    Debug.assert(startByteIndex < length, "invalid 'startByteIndex'")

    length -= startByteIndex
    dataof += startByteIndex
    var scan = dataof
    while length
    {
        if dref scan == delimiter
        {
            if includeDelimiter do
                scan += 1
            return @mkstring(dataof, cast(u64) (scan - dataof))
        }

        scan += 1
        length -= 1
    }

    return @mkstring(dataof, 0)
}
