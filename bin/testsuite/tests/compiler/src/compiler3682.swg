//#global skip

#[Swag.Macro]
func toto(stmt: code)
{
    let a = 10
    #mixin stmt
    #mixin stmt
    #mixin stmt
}

var G: s32

//#[Swag.PrintBc]
#test
{
    toto()
    {
        {
            let fn = closure|a|()
            {
                G += a
            }
            fn()
        }
    }

    @assert(G == 30)
}
