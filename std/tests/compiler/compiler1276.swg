#test
{
    b := 666
    toto()
    @assert(b == 676)
}

#[swag.macro]
func toto()
{
    loop 10
        b += 1
}
