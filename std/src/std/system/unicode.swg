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
        g_subCateg[cast(u32) UnicodeCategory.UppercaseLetter]           = SubCateg.Letter
        g_subCateg[cast(u32) UnicodeCategory.LowercaseLetter]           = SubCateg.Letter
        g_subCateg[cast(u32) UnicodeCategory.TitlecaseLetter]           = SubCateg.Letter
        g_subCateg[cast(u32) UnicodeCategory.ModifierLetter]            = SubCateg.Letter
        g_subCateg[cast(u32) UnicodeCategory.OtherLetter]               = SubCateg.Letter
        g_subCateg[cast(u32) UnicodeCategory.DecimalDigitNumber]        = SubCateg.Digit
        g_subCateg[cast(u32) UnicodeCategory.ConnectorPunctuation]      = SubCateg.Punctuation
        g_subCateg[cast(u32) UnicodeCategory.DashPunctuation]           = SubCateg.Punctuation
        g_subCateg[cast(u32) UnicodeCategory.OpenPunctuation]           = SubCateg.Punctuation
        g_subCateg[cast(u32) UnicodeCategory.InitialQuotePunctuation]   = SubCateg.Punctuation
        g_subCateg[cast(u32) UnicodeCategory.FinalQuotePunctuation]     = SubCateg.Punctuation
        g_subCateg[cast(u32) UnicodeCategory.OtherPunctuation]          = SubCateg.Punctuation
        g_subCateg[cast(u32) UnicodeCategory.MathSymbol]                = SubCateg.Symbol
        g_subCateg[cast(u32) UnicodeCategory.CurrencySymbol]            = SubCateg.Symbol
        g_subCateg[cast(u32) UnicodeCategory.ModifierSymbol]            = SubCateg.Symbol
        g_subCateg[cast(u32) UnicodeCategory.OtherSymbol]               = SubCateg.Symbol
        g_subCateg[cast(u32) UnicodeCategory.SpaceSeparator]            = SubCateg.Separator
        g_subCateg[cast(u32) UnicodeCategory.LineSeparator]             = SubCateg.Separator
        g_subCateg[cast(u32) UnicodeCategory.ParagraphSeparator]        = SubCateg.Separator
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
            return UnicodeCategory.OtherNotAssigned // TODO
        }
        
        func isDigit(c: char)->bool
        {
            if isLatin1(c)
                return latin1.isDigit(c)
            return getCategory(c) == UnicodeCategory.DecimalDigitNumber
        }
    }
}