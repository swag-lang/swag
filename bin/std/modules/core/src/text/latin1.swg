#global public
#global namespace Latin1
using Swag

#[Inline, ConstExpr]
{
    func isAscii(c: u8)         => c <= 0x7F
    func isDigit(c: u8)         => cast(bool) (CharProperties[c] & .Digit)
    func isNumber(c: u8)        => cast(bool) (CharProperties[c] & .Digit)
    func isBinDigit(c: u8)      => cast(bool) (CharProperties[c] & .BinDigit)
    func isHexDigit(c: u8)      => cast(bool) (CharProperties[c] & .HexDigit)
    func isUpper(c: u8)         => cast(bool) (CharProperties[c] & .Upper)
    func isLower(c: u8)         => cast(bool) (CharProperties[c] & .Lower)
    func isPunctuation(c: u8)   => cast(bool) (CharProperties[c] & .Punctuation)
    func isLetter(c: u8)        => cast(bool) (CharProperties[c] & .Letter)
    func isLetterOrDigit(c: u8) => cast(bool) (CharProperties[c] & (.Letter | .Digit))
    func isSpace(c: u8)         => cast(bool) (CharProperties[c] & .Spacing)
    func isControl(c: u8)       => cast(bool) (CharProperties[c] & .Control)
    func isSymbol(c: u8)        => cast(bool) (CharProperties[c] & .Symbol)
    func isWord(c: u8)          => isLetter(c) or isDigit(c) or c == `_`
    func toLower(c: u8)         => isUpper(c) ? (c | 0x20'u8) : c
    func toUpper(c: u8)         => isLower(c) ? (c & ~0x20'u8) : c
}

#[Inline, ConstExpr]
internal func isLowerUtf8(c: u8)->bool
{
    if isAscii(c) and isLower(c):
        return true
    return cast(bool) (CharProperties[c] & .LowerUtf8)
}

#[Inline, ConstExpr]
internal func isUpperUtf8(c: u8)->bool
{
    if isAscii(c) and isUpper(c):
        return true
    return cast(bool) (CharProperties[c] & .UpperUtf8)
}

// Make an utf8 buffer upper case, dealing only with latin1 table
func makeUpper(buffer: [..] u8)
{
    visit &p in buffer where isLowerUtf8(dref p):
        dref p -= 0x20
}

// Make an utf8 buffer lower case, dealing only with latin1 table
func makeLower(buffer: [..] u8)
{
    visit &p in buffer where isUpperUtf8(dref p):
        dref p += 0x20
}

// Compare two utf8 strings, dealing only with latin1 table
// Returns -1, 0 or 1
func compare(src, dst: const [..] u8, ignoreCase = false)->s32
{
    let length      = @countof(src)
    let otherLength = @countof(dst)
    let minLen      = Math.min(length, otherLength)

    var result: s32
    if ignoreCase
    {
        #[Swag.Safety("boundcheck", false)]
        loop i in minLen
        {
            var c1 = src[i]
            var c2 = dst[i]
            if c1 == c2:
                continue

            c1 += isUpperUtf8(c1) ? 0x20 : 0
            c2 += isUpperUtf8(c2) ? 0x20 : 0
            result = cast(s32) c1 - cast(s32) c2
            if result != 0:
                return Math.sign(result)
        }
    }
    else
    {
        result = Memory.compare(@dataof(src), @dataof(dst), minLen)
    }

    if result != 0:
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
    func compareLeft(src, dst: const [..] u8)->s32
    {
        var csrc = @countof(src)
        var cdst = @countof(dst)
        var ai, bi = 0
        loop
        {
            let ca = !csrc ? 0 : src[ai]
            let cb = !cdst ? 0 : dst[bi]

            let dga = isDigit(ca)
            let dgb = isDigit(cb)

            if !dga and !dgb:
                return 0
            if !dga:
                return -1
            if !dgb:
                return 1
            if ca < cb:
                return -1
            if ca > cb:
                return 1

            ai, bi += 1
            csrc, cdst -= 1
        }

        return 0
    }

    func compareRight(src, dst: const [..] u8)->s32
    {
        var bias = 0
        var csrc = @countof(src)
        var cdst = @countof(dst)
        var ai, bi = 0
        loop
        {
            let ca = !csrc ? 0 : src[ai]
            let cb = !cdst ? 0 : dst[bi]

            let dga = isDigit(ca)
            let dgb = isDigit(cb)

            if !dga and !dgb:
                return bias
            if !dga:
                return -1
            if !dgb:
                return 1

            if ca < cb
            {
                if !bias:
                    bias = -1
            }
            elif ca > cb
            {
                if !bias:
                    bias = 1
            }
            elif !ca and !cb:
                return bias

            ai, bi += 1
            csrc, cdst -= 1
        }

        return 0
    }

    let dsrc = @dataof(src)
    let ddst = @dataof(dst)
    var csrc = @countof(src)
    var cdst = @countof(dst)

    var ai, bi = 0
    loop
    {
        var ca = !csrc ? 0 : src[ai]
        var cb = !cdst ? 0 : dst[bi]

        // Skip over leading spaces or zeros
        while isSpace(ca)
        {
            ai += 1
            csrc -= 1
            ca = src[ai]
        }

        while isSpace(cb)
        {
            bi += 1
            cdst -= 1
            cb = dst[bi]
        }

        // Process run of digits
        if isDigit(ca) and isDigit(cb)
        {
            var result = 0
            let sl0    = @mkslice(dsrc + ai, csrc)
            let sl1    = @mkslice(ddst + bi, cdst)
            if ca == `0`'u8 or cb == `0`'u8:
                result = compareLeft(sl0, sl1)
            else:
                result = compareRight(sl0, sl1)
            if result:
                return result
        }

        if !ca and !cb:
            return 0

        if ignoreCase
        {
            ca = toUpper(ca)
            cb = toUpper(cb)
        }

        if ca < cb:
            return -1
        if ca > cb:
            return 1

        ai, bi += 1
        csrc, cdst -= 1
    }

    return 0
}

// Trim the string.
func trim(str: string)->string
{
    var countof = @countof(str)
    var cpt     = 0'u64
    while cpt < countof and Latin1.isSpace(str[cpt]):
        cpt += 1
    while countof > cpt and Latin1.isSpace(str[countof - 1]):
        countof -= 1
    if countof <= cpt:
        return ""
    return @mkstring(@dataof(str) + cpt, countof - cpt)
}
