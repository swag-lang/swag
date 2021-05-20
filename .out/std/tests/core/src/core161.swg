
using core

//#[Swag.printbc]
#test
{
    loc := #location
    res := catch file.readAllBytes(loc.fileName)
    //Console.print("%", res.item0)
}