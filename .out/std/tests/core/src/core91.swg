
using core

#test
{
    ci := core.Globalization.getCultureInfo()
    @assert(ci.numberFormat.decimalSeparator == "."'u8)
}
