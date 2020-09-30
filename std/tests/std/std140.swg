#import "std"
using std.strconv

//#[swag.printbc]
//#[swag.nooptim]
#test
{
    var str: StringBuilder
    str.appendAny(3.14, "20")
    str.append(" ")
    str.appendAny(666)
    console.print("%", str.toString())
}