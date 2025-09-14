using Core

//#[Swag.PrintBc]
//#[Swag.nooptim]
#test
{
    var x: String = "élémenT"
    let y  = x.toUpper(CharacterSet.Latin1)
    @assert(x == "élémenT")
    @assert(y == "ÉLÉMENT")
}
