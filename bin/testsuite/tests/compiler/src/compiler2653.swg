#[Swag.Macro]
func toto(x, y: s64 = 666)
{
    @assert(x == 666)
    @assert(y == 444)
}

//#[Swag.PrintBc]//, Swag.Optimize("", false)]
#test
{
    toto(y: 444)
}
