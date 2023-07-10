#[Swag.Macro]
func toto()
{
    var b = 0
    @assert(b == 0)
}

//#[Swag.PrintBc]
#test
{
    let b = 666
    toto()
    @assert(b == 666)
}