#global testerror "Err0396"

namespace tt
{
    #[Swag.Macro]
    func(ptr: bool, back: bool) opVisit(x: string, stmt: code)
    {
        loop x: #mixin stmt
    }
}

#test
{
    {
        using tt
        visit "toto"
        {

        }
    }
}