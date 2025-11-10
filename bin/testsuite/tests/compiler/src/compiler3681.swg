#[Swag.Macro]
func toto(stmt: #code void)
{
    let a = 666
    #inject(stmt)
}

var G: s32

//#[Swag.PrintBc]
#test
{
    toto()
    {
        let fn = func|a|()
        {
            G += a
        }
        fn()
    }

    @assert(G == 666)
}
