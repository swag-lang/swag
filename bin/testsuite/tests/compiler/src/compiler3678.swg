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
        fn := @(){
            G = 1
        }
        fn()
    }

    @assert(G == 1)
}