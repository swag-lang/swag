#[Swag.Macro]
func toto(stmt: code)
{
    #inject stmt
}

var G: s32

//#[Swag.PrintBc]
#test
{
    toto()
    {
        let fn = func(){
            G = 1
        }
        fn()
    }

    @assert(G == 1)
}