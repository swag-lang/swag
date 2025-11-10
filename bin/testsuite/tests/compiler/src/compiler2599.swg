using Swag

var G = 0

#[Macro]
func toto(stmt: #code void)
{
    func titi()
    {
        #inject(stmt)
    }

    for 10 do
        titi()
}

//#[Swag.PrintBc]
#test
{
    toto()
    {
        G += 1
    }

    @assert(G == 10)
}
