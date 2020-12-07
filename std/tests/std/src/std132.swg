#import "std"
using std.strconv

#test
{
    var builder: StringBuilder
    builder.appendString("toto")
    builder.appendFormat("%", 444)
    builder.appendAny("假"'char)

    var str: String = "mystring"
    builder.appendString(str)

    s := builder.toString();
    //console.write("%\n", s)
    @assert(s == "toto444假mystring")
}