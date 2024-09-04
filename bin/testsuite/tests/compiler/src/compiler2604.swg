#global testerror "Err0394"

namespace tt
{
    #[Swag.Macro]
    func(ptr: bool, back: bool) opVisit(x: string, stmt: code)
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