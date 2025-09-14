using Core.StrConv

#test
{
    var builder: StringBuilder
    builder.appendString("toto")
    builder.appendFormat("%", 444)
    builder.appendAny('假''rune)

    let str: String = "mystring"
    builder.appendString(str)

    let s = builder.toString()
    //Console.write("%\n", s)
    @assert(s == "toto444假mystring")
}
