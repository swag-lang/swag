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
        var a = 666
        var fn = closure|a|(){
            G = a
        }
        fn()
    }

    @assert(G == 666)
}