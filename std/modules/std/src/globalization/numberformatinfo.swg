namespace std.globalization;

// Provides culture-specific information for formatting and parsing numeric values.
public struct NumberFormatInfo
{
    // The character that denotes that the associated number is negative. The default is "-".
    negativeSign:       char = "-"'char

    // The character that denotes that the associated number is positive. The default is "+".
    positiveSign:       char = "+"'char

    // The character to use as the decimal separator in numeric values. The default is ".".
    decimalSeparator:   char = "."'char
}
