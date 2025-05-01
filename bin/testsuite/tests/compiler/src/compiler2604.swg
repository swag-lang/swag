#global testerror "Err0405"

namespace tt
{
    #[Swag.Macro]
    func(ptr: bool, back: bool) opVisit(x: string, stmt: code void)
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