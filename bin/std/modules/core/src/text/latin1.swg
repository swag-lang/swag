#global public
#global namespace Latin1
using Swag

#[Inline, ConstExpr]
{
    func isAscii(c: u8)         => c <= 0x7F
    func isDigit(c: u8)         => cast(bool) (CharProperties[c] & CharAttribute.Digit)
    func isNumber(c: u8)        => cast(bool) (CharProperties[c] & CharAttribute.Digit)
    func isBinDigit(c: u8)      => cast(bool) (CharProperties[c] & CharAttribute.BinDigit)
    func isHexDigit(c: u8)      => cast(bool) (CharProperties[c] & CharAttribute.HexDigit)
    func isUpper(c: u8)         => cast(bool) (CharProperties[c] & CharAttribute.Upper)
    func isLower(c: u8)         => cast(bool) (CharProperties[c] & CharAttribute.Lower)
    func isPunctuation(c: u8)   => cast(bool) (CharProperties[c] & CharAttribute.Punctuation)
    func isLetter(c: u8)        => cast(bool) (CharProperties[c] & CharAttribute.Letter)
    func isLetterOrDigit(c: u8) => cast(bool) (CharProperties[c] & (CharAttribute.Letter | CharAttribute.Digit))
    func isSpace(c: u8)         => cast(bool) (CharProperties[c] & CharAttribute.Spacing)
    func isControl(c: u8)       => cast(bool) (CharProperties[c] & CharAttribute.Control)
    func isSymbol(c: u8)        => cast(bool) (CharProperties[c] & CharAttribute.Symbol)
    func toLower(c: u8)         => isUpper(c) ? (c | 0x20'u8) : c
    func toUpper(c: u8)         => isLower(c) ? (c & ~0x20'u8) : c
}

#[Inline, ConstExpr]
private func isLowerUtf8(c: u8)->bool
{
    if isAscii(c) and isLower(c)
        return true
    return cast(bool) (CharProperties[c] & CharAttribute.LowerUtf8)
}

#[Inline, ConstExpr]
private func isUpperUtf8(c: u8)->bool
{
    if isAscii(c) and isUpper(c)
        return true
    return cast(bool) (CharProperties[c] & CharAttribute.UpperUtf8)
}

// Make an utf8 buffer upper case, dealing only with latin1 table
func makeUpper(buffer: [..] u8)
{
    visit *p: buffer
    {
        if isLowerUtf8(dref p)
            dref p -= 0x20
    }
}

// Make an utf8 buffer lower case, dealing only with latin1 table
func makeLower(buffer: [..] u8)
{
    visit *p: buffer
    {
        if isUpperUtf8(dref p)
            dref p += 0x20
    }
}

// Compare two utf8 strings, dealing only with latin1 table
// Returns -1, 0 or 1
func compare(src, dst: const [..] u8, ignoreCase = false)->s32
{
    length      := @countof(src)
    otherLength := @countof(dst)
    minLen      := Math.min(length, otherLength)

    var result: s32
    if ignoreCase
    {
        #[Swag.Safety("boundcheck", false)]
        loop i: minLen
        {
            c1 := src[i]
            c2 := dst[i]
            if c1 == c2 continue

            c1 += isUpperUtf8(c1) ? 0x20 : 0
            c2 += isUpperUtf8(c2) ? 0x20 : 0
            result = cast(s32) c1 - cast(s32) c2
            if result != 0
                return Math.sign(result)
        }
    }
    else
    {
        result = Memory.compare(@dataof(src), @dataof(dst), minLen)
    }

    if result != 0
        return Math.sign(result)
    return length <=> otherLength
}

// Compare two utf8 strings in a natural way, dealing only with latin1 table
// Returns -1, 0 or 1
//
// :LICENCE
// This code is based on 'natsort' by Martin Pool (https://github.com/sourcefrog/natsort)
// The original code has been modified for Swag
// See LICENCE.md for the corresponding licence.
func compareNatural(src, dst: const [..] u8, ignoreCase = false)->s32
{
    func compareLeft(src, dst: const *u8)->s32
    {
        a := src
        b := dst
        loop
        {
            da  := dref a
            db  := dref b
            dga := isDigit(da)
            dgb := isDigit(db)

            if !dga and !dgb
                return 0
            if !dga
                return -1
            if !dgb
                return 1
            if da < db
                return -1
            if da > db
                return 1
            a, b += 1
        }

        return 0
    }

    func compareRight(src, dst: const *u8)->s32
    {
        bias := 0
        a := src
        b := dst

        loop
        {
            da  := dref a
            db  := dref b
            dga := isDigit(da)
            dgb := isDigit(db)

            if !dga and !dgb
                return bias
            if !dga
                return -1
            if !dgb
                return 1

            if da < db
            {
                if !bias
                    bias = -1
            }
            elif da > db
            {
                if !bias
                    bias = 1
            }
            elif !da and !db
                return bias

            a, b += 1
        }

        return 0;
    }

    dsrc := @dataof(src)
    ddst := @dataof(dst)
    csrc := @countof(src)
    cdst := @countof(dst)

    ai, bi := 0
    loop
    {
        ca := ai == csrc ? 0 : src[ai]
        cb := bi == cdst ? 0 : dst[bi]

        // Skip over leading spaces or zeros
        while isSpace(ca)
        {
            ai += 1
            ca = src[ai]
        }

        while isSpace(cb)
        {
            bi += 1
            cb = dst[bi]
        }

        // Process run of digits
        if isDigit(ca) and isDigit(cb)
        {
            result := 0
            if ca == "0"'u8 or cb == "0"'u8
                result = compareLeft(dsrc + ai, ddst + bi)
            else
                result = compareRight(dsrc + ai, ddst + bi)
            if result
                return result
        }

        if !ca and !cb
            return 0

        if ignoreCase
        {
            ca = toUpper(ca)
            cb = toUpper(cb)
        }

        if ca < cb
            return -1
        if ca > cb
            return 1
        ai, bi += 1
    }

    return 0
}