#[Swag.Macro]
func toto(stmt: code)
{
    #mixin stmt
}

var G: s32

//#[Swag.PrintBc]
#test
{
    toto()
    {
        //var fn: closure(s32)
        var fn = func(val: s32){
            G += val
        }
        fn(1)
    }

    @assert(G == 1)
}