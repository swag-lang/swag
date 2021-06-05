#global testerror

namespace tt
{
    #[Swag.macro]
    func(ptr: bool) opVisit(x: string, stmt: code)
    {
        loop x #mixin stmt
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