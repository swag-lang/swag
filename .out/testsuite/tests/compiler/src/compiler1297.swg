#[Swag.macro]
func toto(x: code)
{
    #macro
    {
        #mixin `x
    }
}

#test
{
    a := 0
    toto()
    {
        a = 666
    }
    @assert(a == 666)
}