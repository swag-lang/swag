#[Swag.Macro]
func toto()
{
    let y = #up x
    @assert(y == 666)
}

#test
{
    let x = 666
    toto()
}
