
using Core

#test
{
    ci := Core.Globalization.getCultureInfo()
    @assert(ci.numberFormat.decimalSeparator == "."'u8)
}
