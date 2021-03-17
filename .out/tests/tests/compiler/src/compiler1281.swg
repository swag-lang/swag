#[swag.mixin]
func toto(x: code)
{
    AAA := #mixin x
    @assert(AAA)
}
    
//#[swag.printbc]
#test
{
    a := 1
    b := 2
    toto(#code a < b)
}