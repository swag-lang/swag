#[Swag.Macro]
func toto(t: s32, x: code)
{
    let xx = t
    @assert(t == 100)
    @assert(xx == 100)
    #macro
    {
        var value = #up xx
        #mixin #up x
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