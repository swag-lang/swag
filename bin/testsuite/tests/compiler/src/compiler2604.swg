#global testerror "Err0430"

namespace tt
{
    #[Swag.Macro]
    func(ptr: bool, back: bool) opVisit(x: string, stmt: code)
    {
        for x: #mixin stmt
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