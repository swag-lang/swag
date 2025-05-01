func tata(x: string)
{
    @assert(x == "titi")
}

#[Swag.Macro]
func toto(x: #block void)
{
    #inject x
}

#test
{
    let s = "titi"
    toto(#code tata(s))
}