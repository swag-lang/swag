namespace std.unicode
{
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

    #[swag.flags]
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

        func toUpper(c: char)->char
        {
            if isAscii(c)
                return cast(char) latin1.toUpper(cast(u8) c)
            return c; // @todo
        }

        func toLower(c: char)->char
        {
            if isAscii(c)
                return cast(char) latin1.toLower(cast(u8) c)
            return c; // @todo
        }
    }
}
