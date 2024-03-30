using Core

#test
{
    let ci = Core.Globalization.getCultureInfo()
    @assert(ci.numberFormat.decimalSeparator == `.`)
}
