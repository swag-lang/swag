
using Core.StrConv

//#[Swag.printbc]
//#[Swag.nooptim]
#test
{
    var str: StringBuilder
    str.appendAny(3.14, "+f2")
    str.appendString(" ")
    str.appendAny(-666, "+010")
    //Console.print("%", str.toString())
}