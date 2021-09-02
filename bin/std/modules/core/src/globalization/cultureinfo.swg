#global namespace Globalization

public struct CultureInfo
{
    numberFormat: NumberFormatInfo
}

var g_CultureInfo: CultureInfo

// Returns the global culture info structure
public func getCultureInfo()->*CultureInfo
{
    return &g_CultureInfo
}
