#import "std"
using std.collections

#test
{
    ci := std.globalization.getCultureInfo()
    @assert(ci.numberFormat.decimalSeparator == "."'char)
}
