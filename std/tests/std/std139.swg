#import "std"
using std

#[swag.printbc]
#test
{
    var x: String = "élémenT"
    y := x.toUpper(CharacterSet.Latin1)
    @assert(x == "élémenT")
    @assert(y == "ÉLÉMENT")
}