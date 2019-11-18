#[swag.macro]
func toto()
{
    b := 0
    @assert(b == 0)
}
    
//#[swag.printbc]
#test
{
    b := 666
    toto()
    @assert(b == 666)
}