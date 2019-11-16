namespace std.latin1
{
	using std.unicode
    private const categoryForLatin1: [] CharCategory = 
    [
        CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control,    // 0000 - 0007
        CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control,    // 0008 - 000F
        CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control,    // 0010 - 0017
        CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control,    // 0018 - 001F
        CharCategory.SpaceSeparator, CharCategory.OtherPunctuation, CharCategory.OtherPunctuation, CharCategory.OtherPunctuation, CharCategory.CurrencySymbol, CharCategory.OtherPunctuation, CharCategory.OtherPunctuation, CharCategory.OtherPunctuation,    // 0020 - 0027
        CharCategory.OpenPunctuation, CharCategory.ClosePunctuation, CharCategory.OtherPunctuation, CharCategory.MathSymbol, CharCategory.OtherPunctuation, CharCategory.DashPunctuation, CharCategory.OtherPunctuation, CharCategory.OtherPunctuation,    // 0028 - 002F
        CharCategory.DecimalDigitNumber, CharCategory.DecimalDigitNumber, CharCategory.DecimalDigitNumber, CharCategory.DecimalDigitNumber, CharCategory.DecimalDigitNumber, CharCategory.DecimalDigitNumber, CharCategory.DecimalDigitNumber, CharCategory.DecimalDigitNumber,    // 0030 - 0037
        CharCategory.DecimalDigitNumber, CharCategory.DecimalDigitNumber, CharCategory.OtherPunctuation, CharCategory.OtherPunctuation, CharCategory.MathSymbol, CharCategory.MathSymbol, CharCategory.MathSymbol, CharCategory.OtherPunctuation,    // 0038 - 003F
        CharCategory.OtherPunctuation, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter,    // 0040 - 0047
        CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter,    // 0048 - 004F
        CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter,    // 0050 - 0057
        CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.OpenPunctuation, CharCategory.OtherPunctuation, CharCategory.ClosePunctuation, CharCategory.ModifierSymbol, CharCategory.ConnectorPunctuation,    // 0058 - 005F
        CharCategory.ModifierSymbol, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter,    // 0060 - 0067
        CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter,    // 0068 - 006F
        CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter,    // 0070 - 0077
        CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.OpenPunctuation, CharCategory.MathSymbol, CharCategory.ClosePunctuation, CharCategory.MathSymbol, CharCategory.Control,    // 0078 - 007F
        CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control,    // 0080 - 0087
        CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control,    // 0088 - 008F
        CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control,    // 0090 - 0097
        CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control, CharCategory.Control,    // 0098 - 009F
        CharCategory.SpaceSeparator, CharCategory.OtherPunctuation, CharCategory.CurrencySymbol, CharCategory.CurrencySymbol, CharCategory.CurrencySymbol, CharCategory.CurrencySymbol, CharCategory.OtherSymbol, CharCategory.OtherSymbol,    // 00A0 - 00A7
        CharCategory.ModifierSymbol, CharCategory.OtherSymbol, CharCategory.LowercaseLetter, CharCategory.InitialQuotePunctuation, CharCategory.MathSymbol, CharCategory.DashPunctuation, CharCategory.OtherSymbol, CharCategory.ModifierSymbol,    // 00A8 - 00AF
        CharCategory.OtherSymbol, CharCategory.MathSymbol, CharCategory.OtherNumber, CharCategory.OtherNumber, CharCategory.ModifierSymbol, CharCategory.LowercaseLetter, CharCategory.OtherSymbol, CharCategory.OtherPunctuation,    // 00B0 - 00B7
        CharCategory.ModifierSymbol, CharCategory.OtherNumber, CharCategory.LowercaseLetter, CharCategory.FinalQuotePunctuation, CharCategory.OtherNumber, CharCategory.OtherNumber, CharCategory.OtherNumber, CharCategory.OtherPunctuation,    // 00B8 - 00BF
        CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter,    // 00C0 - 00C7
        CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter,    // 00C8 - 00CF
        CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.MathSymbol,    // 00D0 - 00D7
        CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.UppercaseLetter, CharCategory.LowercaseLetter,    // 00D8 - 00DF
        CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter,    // 00E0 - 00E7
        CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter,    // 00E8 - 00EF
        CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.MathSymbol,    // 00F0 - 00F7
        CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter, CharCategory.LowercaseLetter,    // 00F8 - 00FF
    ]

	#[swag.inline]        
    public
    {
        func isDigit(c: u8) => c >= cast(u8) '0' && c <= cast(u8) '9'
		func isWhiteSpace(c: u8) => c == cast(u8) ' ' || (c >= cast(u8) '\x09' && c <= cast(u8) '\x0d') || c == cast(u8) '\xa0' || c == cast(u8) '\x85'
        func isUpper(c: u8) => categoryForLatin1[c] == CharCategory.UppercaseLetter
        func isLower(c: u8) => categoryForLatin1[c] == CharCategory.LowercaseLetter
    }
}