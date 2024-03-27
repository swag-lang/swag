using Core

//#[Swag.PrintBc]
#test
{
    var x: String = "élémenT"
    x.makeUpper()
    @assert(x == "ÉLÉMENT")
    x.makeLower()
    @assert(x == "élément")

    var x1: String = "âäàéèëî"
    x1.makeUpper()
    @assert(x1 == "ÂÄÀÉÈËÎ")
    x1.makeLower()
    @assert(x1 == "âäàéèëî")

    var x2: String = "AbCdEf"
    x2.makeUpper()
    @assert(x2 == "ABCDEF")
}
