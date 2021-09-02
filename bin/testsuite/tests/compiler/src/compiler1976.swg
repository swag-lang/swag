//
//#global testerror
using Swag

struct(T) AA1976
{
}

const TITI = 5

impl AA1976
{
    #[Macro, NoReturn]
    func(ptr: bool) opVisit(using self, stmt: code)
    {
        a := TITI
    }
}

#test
{
    var x: AA1976's32
    visit x
    {

    }
}