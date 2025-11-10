#[Swag.Macro]
func toto()
{
    @assert(#up x == 666)
    #up x = 500
}

#test
{
    var x = 666
    toto()
    @assert(x == 500)
}
