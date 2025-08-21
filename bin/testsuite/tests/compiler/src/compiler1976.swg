//
//#global testerror
using Swag

struct(T) AA1976
{
}

const TITI = 5

impl AA1976
{
    #[Macro, CalleeReturn]
    func(ptr: bool, back: bool) opVisit(using me, stmt: #code void)
    {
        var a = TITI
    }
}

#test
{
    var x: AA1976's32
    foreach x
    {

    }
}