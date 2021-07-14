
#global testerror

#[Swag.Macro]
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

//#[Swag.PrintBc]
#test
{
    toto(100)
    {
        @assert(value == 100)
    }
}