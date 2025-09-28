#global skip
var G = 0

//#[Swag.Macro]
func toto(str: string)
    check #isconstexpr(str)
{
    G += 1
    toto(str)
}

func toto(str: string)
    check !#isconstexpr(str)
{
    G += 10
}

#test
{
    toto("a")

    var x = "c"
    toto(x)

    @assert(G == 21)
}