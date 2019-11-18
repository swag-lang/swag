#[swag.macro]
func toto()
{
    y := `x
    @assert(y == 666)
}

#test
{
    x := 666
    toto()
}