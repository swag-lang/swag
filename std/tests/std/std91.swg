#import "std"
using std.text, std.collections

#test
{
    ci := std.globalization.getCultureInfo()
    @assert(ci.numberFormat.decimalSeparator == "."'char)
}
