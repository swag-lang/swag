#[swag.macro]
func toto(stmt: code)
{
    #mixin stmt
}

//#[swag.printbc]
#test
{
    toto()
    {
        x := 0
        alias G1 = x
        G1 = 1
        @assert(x == 1)
    }
}