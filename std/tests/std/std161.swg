#import "std"
using std

//#[swag.printbc]
#test
{
    loc := #location
    res := file.readAllBytes(loc.fileName)
    //console.print("%", res.item0)
}