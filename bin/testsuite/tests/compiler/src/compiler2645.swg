#[Swag.Macro]
func toto(stmt: code)
{
    #inject stmt
}

//#[Swag.PrintBc]
#test
{
    toto()
    {
        var x = 0
        alias G1 = x
        G1 = 1
        @assert(x == 1)
    }
}