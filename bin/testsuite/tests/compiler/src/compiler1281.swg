#[Swag.Mixin]
func toto(x: code)
{
    let AAA = #inject x
    @assert(AAA)
}

//#[Swag.PrintBc]
#test
{
    let a = 1
    let b = 2
    toto(#code a < b)
}