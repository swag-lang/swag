#global testerror

namespace tt
{
    #[swag.macro]
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