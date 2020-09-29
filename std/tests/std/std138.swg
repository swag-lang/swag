#import "std"
using std

//#[swag.printbc]
#test
{
    var x: String = "élémenT"
    x.makeUpper(String.CharacterSet.Latin1)
    @assert(x == "ÉLÉMENT")
    x.makeLower(String.CharacterSet.Latin1)
    @assert(x == "élément")

    var x1: String = "âäàéèëî"
    x1.makeUpper(String.CharacterSet.Latin1)
    @assert(x1 == "ÂÄÀÉÈËÎ")
    x1.makeLower(String.CharacterSet.Latin1)
    @assert(x1 == "âäàéèëî")

    var x2: String = "AbCdEf"
    x2.makeUpper()
    @assert(x2 == "ABCDEF")
}