
using Core

//#[Swag.PrintBc]
#test
{
    var loc = #location
    var res = catch File.readAllBytes(loc.fileName)
    //Console.print("%", res.item0)
}