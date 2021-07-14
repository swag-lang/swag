#[Swag.macro]
func toto(x, y: s64 = 666)
{
    @assert(x == 666)
    @assert(y == 444)
}

//#[Swag.PrintBc]//, Swag.optim("", false)]
#test
{
    toto(y: 444)
}
