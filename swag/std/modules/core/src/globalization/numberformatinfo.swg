#global namespace globalization

// Provides culture-specific information for formatting and parsing numeric values.
public struct NumberFormatInfo
{
    // The byte that denotes that the associated number is negative. The default is "-".
    negativeSign:       u8 = "-"'u8

    // The byte that denotes that the associated number is positive. The default is "+".
    positiveSign:       u8 = "+"'u8

    // The byte to use as the decimal separator in numeric values. The default is ".".
    decimalSeparator:   u8 = "."'u8
}
