var G = 0

//#[Swag.Macro]
func toto(#[Swag.ConstExpr] str: string)
    #selectif @isconstexpr(str)
{
    G += 1
    toto(str)
}

func toto(str: string)
    #selectif !@isconstexpr(str)
{
    G += 10
}

#test
{
    toto("a")

    x := "c"
    toto(x)

    @assert(G == 21)
}