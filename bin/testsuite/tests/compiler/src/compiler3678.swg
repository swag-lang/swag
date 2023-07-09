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
        var fn = func(){
            G = 1
        }
        fn()
    }

    @assert(G == 1)
}