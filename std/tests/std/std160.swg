#import "std"
using std

//#[swag.printbc]
#test
{
    loc := #location
    @assert(file.exists(loc.fileName))

    res := file.readAllText(loc.fileName)
    //console.print("%", res)
}