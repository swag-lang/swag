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
        //var fn: func||(s32)
        let fn = func(val: s32){
            G += val
        }
        fn(1)
    }

    @assert(G == 1)
}