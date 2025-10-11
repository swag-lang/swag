#[Swag.Macro]
func toto(stmt: #code void)
{
    #inject(stmt)
}

var G: s32

//#[Swag.PrintBc]
#test
{
    toto()
    {
        let a  = 666
        let fn = func|a|()
        {
            G = a
        }
        fn()
    }

    @assert(G == 666)
}
