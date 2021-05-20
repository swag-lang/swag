
#global testerror

#[Swag.macro]
func toto(t: s32, x: code)
{
    xx := t
    @assert(t == 100)
    @assert(xx == 100)
    #macro
    {
        value := xx // error is here
        #mixin x
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