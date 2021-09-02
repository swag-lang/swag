
using Core

//#[Swag.PrintBc]
#test
{
    loc := #location
    res := catch File.readAllBytes(loc.fileName)
    //Console.print("%", res.item0)
}