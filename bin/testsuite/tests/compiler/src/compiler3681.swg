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
        fn := closure|a|(){
            G += a
        }
        fn()
    }

    @assert(G == 666)
}