public namespace std.utf8;

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
func countChars(buffer: const [..] u8)->u32
{
    dataof := @dataof(buffer)
    countof := @countof(buffer)

    cpt := 0'u32
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

// Get the address of the previous utf8 character, and the corresponding unicode character
func lastChar(buffer: const [..] u8) -> {c: char, eat: u32}
{
    countof := @countof(buffer)
    if countof == 0
        return @{acast 0, 0}

    dataof := @dataof(buffer)
    ptr := dataof + (countof - 1)
    eat := 1'u32
    while true
    {
        c := :ptr
        if c >> 6 != 2
            break
        if ptr == dataof
            return @{acast 0, 0}
        ptr -= 1
        eat += 1
    }

    return firstChar(@mkslice(ptr, eat))
}

// Get the unicode character pointed by `buffer`, and the address of the next utf8 character
func firstChar(buffer: const [..] u8) -> {c: char, eat: u32}
{
    countof := @countof(buffer)
    if countof == 0
        return @{acast 0, 0}

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

    return @{acast c, 1}
}

// Make an utf8 buffer upper case, dealing only with latin1 table
func makeUpperLatin1(buffer: [..] u8)
{
    p := @dataof(buffer)
    loop @countof(buffer)
    {
        c := :p
        if (0x61 <= c <= 0x7A)
            :p -= 0x20
        else if c == 0xC3
        {
            ce := c
            p += 1
            c = :p
            if 0xA0 <= c <= 0xBE && c != 0xB7
                :p -= 0x20
        }

        p += 1
    }
}

// Make an utf8 buffer lower case, dealing only with latin1 table
func makeLowerLatin1(buffer: [..] u8)
{
    p := @dataof(buffer)
    loop @countof(buffer)
    {
        c := :p
        if (0x41 <= c <= 0x5A)
            :p += 0x20
        else if c == 0xC3
        {
            ce := c
            p += 1
            c = :p
            if 0x80 <= c <= 0x9E && c != 0x97
                :p += 0x20
        }

        p += 1
    }
}
