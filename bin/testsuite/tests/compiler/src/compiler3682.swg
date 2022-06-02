//#global skip

#[Swag.Macro]
func toto(stmt: code)
{
    a := 10
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
            fn := @|a|(){
                G += a
            }
            fn()
        }
    }

    @assert(G == 30)
}