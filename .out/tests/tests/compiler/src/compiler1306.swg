#[Swag.macro]
func toto(t: s32, x: code)
{
    xx := t
    @assert(t == 100)
    @assert(xx == 100)
    #inline
    {
        value := `xx
        #mixin `x
    }
}

//#[Swag.printbc]
#test
{
    toto(100)
    {
        @assert(value == 100)
    }
}