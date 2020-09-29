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

    var x1: String = "âäàéèëî"
    x1.makeUpperLatin1()
    @assert(x1 == "ÂÄÀÉÈËÎ")

    //loop i: x1
    //    @assert(latin1.isUpper(x1[i]))

    x1.makeLowerLatin1()
    @assert(x1 == "âäàéèëî")
}