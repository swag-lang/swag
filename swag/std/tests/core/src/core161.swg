#import "core"
using core

//#[swag.printbc]
#test
{
    loc := #location
    res := file.readAllBytes(loc.fileName) catch {}
    //console.print("%", res.item0)
}