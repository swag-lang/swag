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
        let a = 666
        let fn = closure|a|(){
            G = a
        }
        fn()
    }

    @assert(G == 666)
}