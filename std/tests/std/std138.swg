#import "std"
using std

//#[swag.printbc]
#test
{
    var x: String = "élémenT"
    x.makeUpperLatin1()
    @assert(x == "ÉLÉMENT")
    x.makeLowerLatin1()
    @assert(x == "élément")
}