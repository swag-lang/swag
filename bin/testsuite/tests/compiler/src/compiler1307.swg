
#global testerror "Err0122"

#[Swag.Macro]
func toto(t: s32, x: code)
{
    var xx = t
    @assert(t == 100)
    @assert(xx == 100)
    #macro
    {
        var value = xx // error is here
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