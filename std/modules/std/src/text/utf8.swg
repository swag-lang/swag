public namespace utf8;

// Returns the number of bytes to encode the first character of the
// utf8 buffer
func countBytesAt(buffer: const *u8)->u32
{
    c := :buffer
    count := 1'u32

    if ((c & 0x80) == 0)         count = 1
    else if ((c & 0xE0) == 0xC0) count = 2
    else if ((c & 0xF0) == 0xE0) count = 3
    else if ((c & 0xF8) == 0xF0) count = 4

    return count
}

// Returns the byte index of the given character index
func byteIndex(buffer: const [..] u8, charIndex: u32) -> {index: u32, success: bool}
{
    countof := @countof(buffer)
    if !countof return @{0'u32, false}
    if !charIndex return @{0'u32, true}

    dataof := @dataof(buffer)

    resultChar := 0'u32
    resultByte := 0'u32
    while countof && resultChar != charIndex
    {
        val := countBytesAt(dataof)
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
        val := countBytesAt(dataof)
        debug.assert(val <= countof, "invalid utf8 character")
        cpt += 1
        dataof += val
        countof -= val
    }

    return cpt
}

// Get the previous utf8 character, and the number of bytes to encode it
func lastChar(buffer: const [..] u8) -> {c: char, eat: u32}
{
    countof := @countof(buffer)
    if countof == 0
        return @{autocast 0, 0}

    dataof := @dataof(buffer)
    ptr := dataof + (countof - 1)
    eat := 1'u32
    while true
    {
        c := :ptr
        if c >> 6 != 2
            break
        if ptr == dataof
            return @{autocast 0, 0}
        ptr -= 1
        eat += 1
    }

    return firstChar(@mkslice(ptr, cast(uint) eat))
}

// Get the unicode character pointed by 'buffer', and the number of bytes to encode it
func firstChar(buffer: const [..] u8) -> {c: char, eat: u32}
{
    countof := @countof(buffer)
    if countof == 0
        return @{autocast 0, 0}

    dataof := @dataof(buffer)
    c := cast(char) :dataof
    dataof += 1

    // Ascii
    if ((c & 0x80) == 0)
        return @{c, 1}

    // Utf8
    var wc: char = ?
    if ((c & 0xE0) == 0xC0)
    {
        debug.assert(countof >= 2, "invalid utf8 character")
        wc = (c & 0x1F) << 6;
        c = cast(char) :dataof
        wc |= (c & 0x3F);
        return @{wc, 2}
    }

    if ((c & 0xF0) == 0xE0)
    {
        debug.assert(countof >= 3, "invalid utf8 character")
        wc = (c & 0xF) << 12;
        wc |= cast(char) (dataof[0] & 0x3F) << 6;
        wc |= cast(char) (dataof[1] & 0x3F);
        return @{wc, 3}
    }

    if ((c & 0xF8) == 0xF0)
    {
        debug.assert(countof >= 4, "invalid utf8 character")
        wc = (c & 0x7) << 18;
        wc |= cast(char) (dataof[0] & 0x3F) << 12;
        wc |= cast(char) (dataof[1] & 0x3F) << 6;
        wc |= cast(char) (dataof[2] & 0x3F);
        return @{wc, 4}
    }

    if ((c & 0xFC) == 0xF8)
    {
        wc = (c & 0x3) << 24;
        wc |= (c & 0x3F) << 18;
        wc |= (c & 0x3F) << 12;
        wc |= (c & 0x3F) << 6;
        wc |= (c & 0x3F);
        return @{wc, 1}
    }

    if ((c & 0xFE) == 0xFC)
    {
        wc = (c & 0x1) << 30;
        wc |= (c & 0x3F) << 24;
        wc |= (c & 0x3F) << 18;
        wc |= (c & 0x3F) << 12;
        wc |= (c & 0x3F) << 6;
        wc |= (c & 0x3F);
        return @{wc, 1}
    }

    return @{autocast c, 1}
}

// Find one of the characters in 'what', and returns the byte index of it
func indexOf(src: const [..] u8, what: const [..] char, startByteIndex = 0'uint) -> {index: uint, success: bool}
{
    length := @countof(src)
    if !length return @{0, false}
    debug.assert(startByteIndex < length, "invalid 'startByteIndex'")

    countof := @countof(what)
    if !countof return @{0, false}

    idx := startByteIndex
    scan := @dataof(src) + startByteIndex
    remain := length - startByteIndex
    while countof <= remain
    {
        res := utf8.firstChar(cast(string) @mkslice(scan, remain))
        if !res.eat
            break
        if contains(what, res.c)
            return @{idx, true}
        scan, idx += res.eat
        remain -= res.eat
    }

    return @{0, false}
}

// Find the given string, and returns the byte index of it
func indexOf(src: const [..] u8, what: string, startByteIndex = 0'uint, comparisonType = ComparisonType.Latin1) -> {index: uint, success: bool}
{
    length := @countof(src)
    if !length return @{0, false}
    debug.assert(startByteIndex < length, "invalid 'startByteIndex'")

    countof := @countof(what)
    if !countof || countof > (length - startByteIndex) return @{0, false}

    idx := startByteIndex
    scan := @dataof(src) + startByteIndex
    remain := length - startByteIndex
    while countof <= remain
    {
        res := compare(cast(string) @mkslice(scan, countof), what, comparisonType)
        if res == 0 return @{idx, true}
        scan, idx += 1
        remain -= 1
    }

    return @{0, false}
}

// Return true if the string starts with 'str'
func startsWith(src: const [..] u8, str: string, comparisonType = utf8.ComparisonType.Latin1)->bool
{
    length := @countof(src)
    countof := @countof(str)
    if countof > length return false
    buffer := @dataof(src)
    return compare(cast(string) @mkslice(buffer, countof), str, comparisonType) == 0
}

// Returns the last index (in bytes) of a character
func lastIndexOf(src: const [..] u8, what: const [..] char) -> {index: uint, success: bool}
{
    length := @countof(src)
    if !length return @{0, false}
    dataof := @dataof(src)

    byteIdx := length
    while true
    {
        slice := @mkslice(dataof, byteIdx)
        (c, eat) := lastChar(slice)
        if eat == 0
            return @{0, false}

        byteIdx -= eat
        if contains(what, c)
            return @{byteIdx, true}
    }

    return @{0, false}
}

// Return true if the string ends with 'str'
func endsWith(src: const [..] u8, str: string, comparisonType = utf8.ComparisonType.Latin1)->bool
{
    length := @countof(src)
    countof := @countof(str)
    if countof > length return false
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
            if eat1 == 0 || eat2 == 0 break

            if c1 != c2
            {
                c1 = unicode32.toLower(c1)
                c2 = unicode32.toLower(c2)
                result = cast(s32) c1 - cast(s32) c2
                if result != 0 return math.sign(result)
            }

            length -= eat1
            otherLength -= eat2
            data += eat1
            otherData += eat2
        }

        if !eat1 && eat2 return -1
        if eat1 && !eat2 return 1
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
    if !count return

    byteIdx := 0'uint
    charIdx := 0'uint
    scan := @dataof(buffer)
    while count
    {
        slice := @mkslice(scan, count)
        var (c, eat) = firstChar(slice)
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
func fromChar(dest: [..] u8, src: char)->u32
{
    debug.assert(@countof(dest) >= 4, "destination buffer is too small")

    if (src <= 0x7F)
    {
        dest[0] = cast(u8) src
        return 1
    }

    if (src <= 0x07FF)
    {
        dest[0] = cast(u8) (((src >> 6) & 0x1F) | 0xC0)
        dest[1] = cast(u8) (((src >> 0) & 0x3F) | 0x80)
        return 2
    }

    if (src <= 0xFFFF)
    {
        dest[0] = cast(u8) (((src >> 12) & 0x0F) | 0xE0)
        dest[1] = cast(u8) (((src >> 6) & 0x3F) | 0x80)
        dest[2] = cast(u8) (((src >> 0) & 0x3F) | 0x80)
        return 3
    }

    if (src <= 0x10FFFF)
    {
        dest[0] = cast(u8) (((src >> 18) & 0x07) | 0xF0)
        dest[1] = cast(u8) (((src >> 12) & 0x3F) | 0x80)
        dest[2] = cast(u8) (((src >> 6) & 0x3F) | 0x80)
        dest[3] = cast(u8) (((src >> 0) & 0x3F) | 0x80)
        return 4
    }

    dest[0] = cast(u8) 0xEF;
    dest[1] = cast(u8) 0xBF;
    dest[2] = cast(u8) 0xBD;
    return 3
}

// Convert a unicode 32 array to an utf8 buffer
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