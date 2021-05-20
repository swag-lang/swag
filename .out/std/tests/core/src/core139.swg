
using Core

//#[Swag.printbc]
//#[Swag.nooptim]
#test
{
    var x: String = "élémenT"
    y := x.toUpper(CharacterSet.Latin1)
    @assert(x == "élémenT")
    @assert(y == "ÉLÉMENT")
}