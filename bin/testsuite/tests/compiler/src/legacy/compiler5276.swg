struct A {}

impl A
{
    #[Swag.Macro]
    mtd(ptr: bool, back: bool) opVisit(stmt: #code void)
    {
        #inject(stmt)
    }
}

func toto()->A throw
{
    return {}
}

var G = 0
#test
{
    foreach assume toto()
    {
        G += 1
    }
    @assert(G == 1)
}
