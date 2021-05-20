
using core

//#[Swag.printbc]
#test
{
    loc := #location
    res := catch File.readAllBytes(loc.fileName)
    //Console.print("%", res.item0)
}