namespace std.unicode32;

public enum UnicodeCategory : u8
{
    UppercaseLetter
    LowercaseLetter
    TitlecaseLetter
    ModifierLetter
    OtherLetter
    NonSpacingMark
    SpacingCombiningMark
    EnclosingMark
    DecimalDigitNumber
    LetterNumber
    OtherNumber
    SpaceSeparator
    LineSeparator
    ParagraphSeparator
    Control
    Format
    Surrogate
    PrivateUse
    ConnectorPunctuation
    DashPunctuation
    OpenPunctuation
    ClosePunctuation
    InitialQuotePunctuation
    FinalQuotePunctuation
    OtherPunctuation
    MathSymbol
    CurrencySymbol
    ModifierSymbol
    OtherSymbol
    OtherNotAssigned
}

#[swag.enumflags]
enum SubCateg
{
    None
    Letter
    Digit
    Punctuation
    Symbol
    Separator
}

var g_subCateg: [@countof(UnicodeCategory)] SubCateg = ?

#init
{
    using UnicodeCategory, SubCateg
    g_subCateg[cast(u32) UppercaseLetter]           = Letter
    g_subCateg[cast(u32) LowercaseLetter]           = Letter
    g_subCateg[cast(u32) TitlecaseLetter]           = Letter
    g_subCateg[cast(u32) ModifierLetter]            = Letter
    g_subCateg[cast(u32) OtherLetter]               = Letter
    g_subCateg[cast(u32) DecimalDigitNumber]        = Digit
    g_subCateg[cast(u32) ConnectorPunctuation]      = Punctuation
    g_subCateg[cast(u32) DashPunctuation]           = Punctuation
    g_subCateg[cast(u32) OpenPunctuation]           = Punctuation
    g_subCateg[cast(u32) InitialQuotePunctuation]   = Punctuation
    g_subCateg[cast(u32) FinalQuotePunctuation]     = Punctuation
    g_subCateg[cast(u32) OtherPunctuation]          = Punctuation
    g_subCateg[cast(u32) MathSymbol]                = Symbol
    g_subCateg[cast(u32) CurrencySymbol]            = Symbol
    g_subCateg[cast(u32) ModifierSymbol]            = Symbol
    g_subCateg[cast(u32) OtherSymbol]               = Symbol
    g_subCateg[cast(u32) SpaceSeparator]            = Separator
    g_subCateg[cast(u32) LineSeparator]             = Separator
    g_subCateg[cast(u32) ParagraphSeparator]        = Separator
}

#[swag.inline, swag.constexpr]
public
{
    func isLatin1(c: char)  => c <= 0xFF
    func isAscii(c: char)   => c <= 0x7F
}

#[swag.constexpr]
public
{
    func isLetter(uc: UnicodeCategory)          => cast(bool) (g_subCateg[cast(u32) uc] & SubCateg.Letter)
    func isLetterOrDigit(uc: UnicodeCategory)   => cast(bool) (g_subCateg[cast(u32) uc] & (SubCateg.Letter | SubCateg.Digit))
    func isPunctuation(uc: UnicodeCategory)     => cast(bool) (g_subCateg[cast(u32) uc] & SubCateg.Punctuation)
    func isSymbol(uc: UnicodeCategory)          => cast(bool) (g_subCateg[cast(u32) uc] & SubCateg.Symbol)
    func isSeparator(uc: UnicodeCategory)       => cast(bool) (g_subCateg[cast(u32) uc] & SubCateg.Separator)

    func getCategory(c: char)->UnicodeCategory
    {
        if isLatin1(c)
            return latin1.charCategory[cast(u8) c]
        return UnicodeCategory.OtherNotAssigned // @todo
    }

    func isWhiteSpace(c: char)->bool
    {
        if isLatin1(c)
            return latin1.isWhiteSpace(cast(u8) c)
        return isSeparator(getCategory(c))
    }

    func isLetter(c: char)->bool
    {
        if isLatin1(c)
            return latin1.isLetter(cast(u8) c)
        return isLetter(getCategory(c))
    }

    func isLetterOrDigit(c: char)->bool
    {
        if isLatin1(c)
            return latin1.isLetterOrDigit(cast(u8) c)
        return isLetterOrDigit(getCategory(c))
    }

    func isDigit(c: char)->bool
    {
        if isLatin1(c)
            return latin1.isDigit(cast(u8) c)
        return getCategory(c) == UnicodeCategory.DecimalDigitNumber
    }
}

// Convert an utf8 buffer to a 32 bits character sequence, and returns the
// number of valid elements in the destination buffer
public func fromUtf8(dest: [..] char, src: const [..] u8)->u32
{
    count := 0'u32
    srcPtr := @dataof(src)
    destPtr := @dataof(dest)
    srcLength := @countof(src)

    (c, eat) := utf8.firstChar(src)
    while eat != 0
    {
        debug.assert(count != @countof(dest), "destination buffer is too small")

        :destPtr = c
        destPtr, count += 1

        srcPtr += eat
        srcLength -= eat
        (c, eat) = utf8.firstChar(@mkslice(srcPtr, srcLength))
    }

    return count
}
