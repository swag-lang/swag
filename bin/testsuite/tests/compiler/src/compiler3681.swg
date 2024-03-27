#[Swag.Macro]
func toto(stmt: code)
{
    let a = 666
    #mixin stmt
}

var G: s32

//#[Swag.PrintBc]
#test
{
    toto()
    {
        let fn = closure|a|()
        {
            G += a
        }
        fn()
    }

    @assert(G == 666)
}
