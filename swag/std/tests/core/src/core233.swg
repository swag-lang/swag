#import "core"
using core

//#[swag.printbc]
func toto()->[2] String
{
    return @[String.from("a"),String.from("b")]
}

//#[swag.printbc]
#test
{
    x := toto()
    @assert(x[0] == "a")
    @assert(x[1] == "b")
}