#[Swag.Macro]
func toto(stmt: code)
{
    #mixin stmt
}

//#[Swag.PrintBc]
#test
{
    toto()
    {
        var x = 0
        namealias G1 = x
        G1 = 1
        @assert(x == 1)
    }
}