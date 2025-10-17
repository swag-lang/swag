#global testerror "Err0406"

namespace tt
{
    #[Swag.Macro]
    func(ptr: bool, back: bool) opVisit(x: string, stmt: #code void)
    {
        for x do
            #inject(stmt)
    }
}

#test
{
    {
        using tt
        foreach "toto"
        {
        }
    }
}
