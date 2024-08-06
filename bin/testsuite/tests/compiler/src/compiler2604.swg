#global testerror "Err0445"

namespace tt
{
    #[Swag.Macro]
    func(ptr: bool, back: bool) opVisit(x: string, stmt: code)
    {
        loop x do #mixin stmt
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