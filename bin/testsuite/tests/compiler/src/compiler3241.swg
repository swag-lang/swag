func tata(x: string)
{
    @assert(x == "titi")
}

#[Swag.Macro]
func toto(x: code)
{
    #mixin x
}

#test
{
    var s = "titi"
    toto(#code tata(s))
}