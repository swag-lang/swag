var G = 0

#[Swag.Macro]
func toto(x: string)
{
    func(v: string) titi()
    {
        #if v == "titi" do
            G += 1
        #if v == "tata" do
            G += 2
    }

    titi'x()
}

#test
{
    toto("titi")
    toto("tata")
    @assert(G == 3)
}
