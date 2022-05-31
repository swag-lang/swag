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
        a := 666
        fn := @|a|(){
            G = a
        }
        fn()
    }

    @assert(G == 666)
}