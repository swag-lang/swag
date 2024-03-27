#[Swag.Macro]
func toto(x: code)
{
    #macro
    {
        #mixin #up x
    }
}

#test
{
    var a = 0
    toto()
    {
        a = 666
    }
    @assert(a == 666)
}
