//#global skip

#[Swag.Macro]
func toto(stmt: #code void)
{
    #inject(stmt)
    #inject(stmt)
    #inject(stmt)
}

var G: s32

//#[Swag.PrintBc]
#test
{
    toto()
    {
        {
            let fn = func(a: s32)
            {
                G += a
            }
            fn(10)
        }
    }

    @assert(G == 30)
}
