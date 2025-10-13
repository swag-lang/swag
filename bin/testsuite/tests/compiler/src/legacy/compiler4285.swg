using Swag

var G = 0

#[Macro, Swag.Overload]
func toto(x: string)
    verify #isconstexpr(x)
{
    G += 1

    #[Match(.Me|.Where, false)]
    toto(x)
}

#[Swag.Overload]
func toto(x: string)
    verify !#isconstexpr(x)
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
