using Swag

var G = 0

#[Macro]
func toto(x: string)
    #selectifx @isconstexpr(x)
{
    G += 1

    #[Match("self|selectif", false)]
    toto(x)
}

func toto(x: string)
    #selectifx !@isconstexpr(x)
{
    G += 2
}

#test
{
    toto("a")
    @assert(G == 3)

    v := "coucou"
    toto(v)
    @assert(G == 5)
}