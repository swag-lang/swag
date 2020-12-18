#import "core"
using core

#test
{
    ci := core.globalization.getCultureInfo()
    @assert(ci.numberFormat.decimalSeparator == "."'char)
}
