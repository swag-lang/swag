
using core

//#[swag.printbc]
#test
{
    loc := #location
    res := catch file.readAllBytes(loc.fileName)
    //console.print("%", res.item0)
}