
using core

//#[Swag.printbc]
#test
{
    var x: String = "élémenT"
    x.makeUpper(CharacterSet.Latin1)
    @assert(x == "ÉLÉMENT")
    x.makeLower(CharacterSet.Latin1)
    @assert(x == "élément")

    var x1: String = "âäàéèëî"
    x1.makeUpper(CharacterSet.Latin1)
    @assert(x1 == "ÂÄÀÉÈËÎ")
    x1.makeLower(CharacterSet.Latin1)
    @assert(x1 == "âäàéèëî")

    var x2: String = "AbCdEf"
    x2.makeUpper()
    @assert(x2 == "ABCDEF")
}