#[swag.macro]
func toto()
{
    loop 10
        b += 1
}
    
#test
{
    b := 666
    toto()
    @assert(b == 676)
}