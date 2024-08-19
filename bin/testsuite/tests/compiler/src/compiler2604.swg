#global testerror "Err0408"

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