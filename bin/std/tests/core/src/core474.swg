using Core

var g: bool

func toto(x: string)
{
    @assert(x == "titi")
}

//#[Swag.PrintBc]
#test
{
    var a: Array'String
    a.add("toto")
    toto(g ? a[0] : String.from("titi"))
}
