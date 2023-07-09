//#global skip

#[Swag.Macro]
func toto(stmt: code)
{
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
            var fn = func(a: s32){
                G += a
            }
            fn(10)
        }
    }

    @assert(G == 30)
}