//#module "compiler1974"
//#unittest error
using swag

struct(T) AA1976
{
}

impl AA1976
{
    const TITI = 5

    #[macro, noreturn]
    func(ptr: bool) opVisit(using self, stmt: code)
    {
        a := TITI
    }
}

#test
{
    var x: AA1976's32
    visit i: x
    {

    }
}