
using Core.StrConv

#test
{
    var builder: StringBuilder
    builder.appendString("toto")
    builder.appendFormat("%", 444)
    builder.appendAny("假"'rune)

    var str: String = "mystring"
    builder.appendString(str)

    s := builder.toString();
    //Console.write("%\n", s)
    @assert(s == "toto444假mystring")
}