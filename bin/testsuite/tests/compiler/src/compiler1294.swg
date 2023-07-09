#[Swag.Macro]
func toto()
{
    var y = #up x
    @assert(y == 666)
}

#test
{
    var x = 666
    toto()
}