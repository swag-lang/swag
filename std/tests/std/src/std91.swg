#import "std"
using std

#test
{
    ci := std.globalization.getCultureInfo()
    @assert(ci.numberFormat.decimalSeparator == "."'char)
}
