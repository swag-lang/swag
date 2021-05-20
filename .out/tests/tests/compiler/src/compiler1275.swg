#[Swag.macro]
func toto()
{
    b := 0
    @assert(b == 0)
}

//#[Swag.printbc]
#test
{
    b := 666
    toto()
    @assert(b == 666)
}