
using Core

#test
{
    var ci = Core.Globalization.getCultureInfo()
    @assert(ci.numberFormat.decimalSeparator == "."'u8)
}
