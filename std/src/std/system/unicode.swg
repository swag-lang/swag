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
    
    #[swag.inline, swag.constexpr]
    public
    {
        func isLatin1(c: char) => c <= 0xFF
        func isAscii(c: char) => c <= 0x7F
    } 

    #[swag.constexpr]
    public
    {
        func isLetter(uc: UnicodeCategory)->bool
        {
            return  uc == UnicodeCategory.UppercaseLetter ||
                    uc == UnicodeCategory.LowercaseLetter ||
                    uc == UnicodeCategory.TitlecaseLetter ||
                    uc == UnicodeCategory.ModifierLetter ||
                    uc == UnicodeCategory.OtherLetter
        }
        
        func isLetterOrDigit(uc: UnicodeCategory)->bool
        {
            return  uc == UnicodeCategory.UppercaseLetter ||
                    uc == UnicodeCategory.LowercaseLetter ||
                    uc == UnicodeCategory.TitlecaseLetter ||
                    uc == UnicodeCategory.ModifierLetter ||
                    uc == UnicodeCategory.OtherLetter ||
                    uc == UnicodeCategory.DecimalDigitNumber
        }        
        
        func isPunctuation(uc: UnicodeCategory)->bool
        {
            return  uc == UnicodeCategory.ConnectorPunctuation ||
                    uc == UnicodeCategory.DashPunctuation ||
                    uc == UnicodeCategory.OpenPunctuation ||
                    uc == UnicodeCategory.ClosePunctuation ||
                    uc == UnicodeCategory.InitialQuotePunctuation ||
                    uc == UnicodeCategory.FinalQuotePunctuation ||
                    uc == UnicodeCategory.OtherPunctuation
        }        

        func getCategory(c: char)->UnicodeCategory
        {
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