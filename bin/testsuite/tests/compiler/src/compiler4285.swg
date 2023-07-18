using Swag

var G = 0

#[Macro]
func toto(x: string)
    #validifx @isconstexpr(x)
{
    G += 1

    #[Match("self|validif", false)]
    toto(x)
}

func toto(x: string)
    #validifx !@isconstexpr(x)
{
    G += 2
}

#test
{
    toto("a")
    @assert(G == 3)

    var v = "coucou"
    v = "coucou1"
    toto(v)
    @assert(G == 5)
}