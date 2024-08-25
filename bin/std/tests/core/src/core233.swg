using Core

//#[Swag.PrintBc, Swag.Optimize("", false)]
func toto()->[2] String
{
    return [String.from("a"), String.from("b")]
}

//#[Swag.PrintBc]
#test
{
    var x = toto()
    @assert(x[0] == "a")
    @assert(x[1] == "b")
}
