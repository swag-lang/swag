public namespace std.utf8;

// Returns the number of unicode characters in an utf8 buffer
func countChar(buffer: const *u8)->u32
{
    if buffer == null
        return 0
    ptr := buffer
    cpt := 0'u32
    var c: char = ?
    while true
    {
        (ptr, c) = nextChar(ptr)
        if !c
            return cpt
        cpt += 1
    }

    return 0
}

// Get the address of the previous utf8 character, and the corresponding unicode character
func previousChar(start, buffer: const *u8) -> {const *u8, char}
{
    if buffer == null || buffer == start
        return @{buffer, acast 0}

    ptr := buffer - 1
    while true
    {
        c := :ptr
        if c >> 6 != 2
            break
        if ptr == start
            return @{start, acast 0}
        ptr -= 1
    }

    result := nextChar(ptr)
    return @{ptr, result.item1}
}

// Get the unicode character pointed by `buffer`, and the address of the next utf8 character
func nextChar(buffer: const *u8) -> {const *u8, char}
{
    if buffer == null
        return @{null, acast 0}

    ptr := buffer
    c := cast(char) :ptr
    if c == 0
        return @{ptr, acast 0}
    ptr += 1

    // Ascii
    if ((c & 0x80) == 0)
    {
        return @{ptr, c}
    }

    // Utf8
    var wc: char = ?
    if ((c & 0xE0) == 0xC0)
    {
        wc = (c & 0x1F) << 6;
        c = cast(char) :ptr
        ptr += 1
        wc |= (c & 0x3F);
        return @{ptr, wc}
    }

    if ((c & 0xF0) == 0xE0)
    {
        wc = (c & 0xF) << 12;
        wc |= cast(char) (ptr[0] & 0x3F) << 6;
        wc |= cast(char) (ptr[1] & 0x3F);
        ptr += 2;
        return @{ptr, wc}
    }

    if ((c & 0xF8) == 0xF0)
    {
        wc = (c & 0x7) << 18;
        wc |= cast(char) (ptr[0] & 0x3F) << 12;
        wc |= cast(char) (ptr[1] & 0x3F) << 6;
        wc |= cast(char) (ptr[2] & 0x3F);
        ptr += 3;
        return @{ptr, wc}
    }

    if ((c & 0xFC) == 0xF8)
    {
        wc = (c & 0x3) << 24;
        wc |= (c & 0x3F) << 18;
        wc |= (c & 0x3F) << 12;
        wc |= (c & 0x3F) << 6;
        wc |= (c & 0x3F);
        return @{ptr, wc}
    }

    if ((c & 0xFE) == 0xFC)
    {
        wc = (c & 0x1) << 30;
        wc |= (c & 0x3F) << 24;
        wc |= (c & 0x3F) << 18;
        wc |= (c & 0x3F) << 12;
        wc |= (c & 0x3F) << 6;
        wc |= (c & 0x3F);
        return @{ptr, wc}
    }

    return @{ptr, acast c}
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
