namespace std.unicode
{      
    public enum UnicodeCategory : u8
    {
        UppercaseLetter = 0
        LowercaseLetter = 1
        TitlecaseLetter = 2
        ModifierLetter = 3
        OtherLetter = 4
        NonSpacingMark = 5
        SpacingCombiningMark = 6
        EnclosingMark = 7
        DecimalDigitNumber = 8
        LetterNumber = 9
        OtherNumber = 10
        SpaceSeparator = 11
        LineSeparator = 12
        ParagraphSeparator = 13
        Control = 14
        Format = 15
        Surrogate = 16
        PrivateUse = 17
        ConnectorPunctuation = 18
        DashPunctuation = 19
        OpenPunctuation = 20
        ClosePunctuation = 21
        InitialQuotePunctuation = 22
        FinalQuotePunctuation = 23
        OtherPunctuation = 24
        MathSymbol = 25
        CurrencySymbol = 26
        ModifierSymbol = 27
        OtherSymbol = 28
        OtherNotAssigned = 29
    }
    
    #[swag.flags]
    enum SubCateg
    {
        None
        Letter
        Digit
        Punctuation
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
        func isLetter(uc: UnicodeCategory)->bool
        {
            return cast(bool) (g_subCateg[cast(u32) uc] & SubCateg.Letter)
        }
        
        func isLetterOrDigit(uc: UnicodeCategory)->bool
        {
            return cast(bool) (g_subCateg[cast(u32) uc] & (SubCateg.Letter | SubCateg.Digit))
        }
        
        func isPunctuation(uc: UnicodeCategory)->bool
        {
            return cast(bool) (g_subCateg[cast(u32) uc] & SubCateg.Punctuation)
        }        

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