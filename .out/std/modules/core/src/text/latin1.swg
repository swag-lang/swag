#global public
#global namespace Latin1
using Swag

#[inline, ConstExpr]
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

#[inline, ConstExpr]
{
    private func isLowerUtf8(c: u8)->bool
    {
        if isAscii(c) and isLower(c) return true
        return cast(bool) (CharProperties[c] & CharAttribute.LowerUtf8)
    }

    private func isUpperUtf8(c: u8)->bool
    {
        if isAscii(c) and isUpper(c) return true
        return cast(bool) (CharProperties[c] & CharAttribute.UpperUtf8)
    }
}

// Make an utf8 buffer upper case, dealing only with latin1 table
func makeUpper(buffer: [..] u8)
{
    p := @dataof(buffer)
    loop @countof(buffer)
    {
        c := dref p
        if isLowerUtf8(c)
            dref p -= 0x20
        p += 1
    }
}

// Make an utf8 buffer lower case, dealing only with latin1 table
func makeLower(buffer: [..] u8)
{
    p := @dataof(buffer)
    loop @countof(buffer)
    {
        c := dref p
        if isUpperUtf8(c)
            dref p += 0x20
        p += 1
    }
}

// Compare two latin1 strings, and returns -1, 0 or 1
func compare(src, dst: const [..] u8, ignoreCase = false)->s32
{
    length := @countof(src)
    otherLength := @countof(dst)
    minLen := Math.min(length, otherLength)

    var result: s32
    if ignoreCase
    {
        #[Swag.safety("boundcheck", false)]
        loop i: minLen
        {
            c1 := src[i]
            c2 := dst[i]
            if c1 == c2 continue

            c1 += isUpperUtf8(c1) ? 0x20 : 0
            c2 += isUpperUtf8(c2) ? 0x20 : 0
            result = cast(s32) c1 - cast(s32) c2
            if result != 0 return Math.sign(result)
        }
    }
    else
    {
        result = Memory.compare(@dataof(src), @dataof(dst), minLen)
    }

    if result != 0 return Math.sign(result)
    return length <=> otherLength
}