
using core

//#[Swag.printbc, Swag.optim("", false)]
func toto()->[2] String
{
    return @[String.from("a"),String.from("b")]
}

//#[Swag.printbc]
#test
{
    x := toto()
    @assert(x[0] == "a")
    @assert(x[1] == "b")
}