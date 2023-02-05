#[Swag.Macro]
func toto()
{
    y := #up x
    @assert(y == 666)
}

#test
{
    x := 666
    toto()
}