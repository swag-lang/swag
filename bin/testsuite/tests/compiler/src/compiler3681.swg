#[Swag.Macro]
func toto(stmt: code)
{
    a := 666
    #mixin stmt
}

var G: s32

//#[Swag.PrintBc]
#test
{
    toto()
    {
        fn := @|a|(){
            G += a
        }
        fn()
    }

    @assert(G == 666)
}