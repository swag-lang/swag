#[Swag.mixin]
func toto(x: code)
{
    AAA := #mixin x
    @assert(AAA)
}

//#[Swag.printbc]
#test
{
    a := 1
    b := 2
    toto(#code a < b)
}