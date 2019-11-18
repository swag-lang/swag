#[swag.macro]
func toto(x: code)
{
    AAA := #insert x
    @assert(AAA)
}
    
//#[swag.printbc]
#test
{
    a := 1
    b := 2
    toto(#code a < b)
}