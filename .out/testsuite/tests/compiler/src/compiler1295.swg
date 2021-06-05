#[Swag.macro]
func toto()
{
    @assert(`x == 666)
    `x = 500
}

#test
{
    x := 666
    toto()
    @assert(x == 500)
}