#[Swag.Macro]
func toto(x: string)
{
    func(v: string) titi()
    {
        @assert(v == "titi")
    }

    titi'x()
}

#test
{
    toto("titi")
}
