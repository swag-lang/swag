#global testerror "Err0405"

namespace tt
{
    #[Swag.Macro]
    func(ptr: bool, back: bool) opVisit(x: string, stmt: #block void)
    {
        for x: #inject stmt
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