#import "std"
using std.strconv

#test
{
    var builder: StringBuilder
    builder.append("toto")
    builder.appendFormat("%", 444)

    var str: String = "mystring"
    builder.append(str)

    s := builder.toString();
    //console.write("%\n", s)
    @assert(s == "toto444mystring")
}