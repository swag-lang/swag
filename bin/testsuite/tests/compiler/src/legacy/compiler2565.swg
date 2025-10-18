#global testerror "Err0228"

struct ErrTiti {}

struct AA {}

impl AA
{
    func opEquals(me, other: AA)->bool
    {
        throw ErrTiti{}
        return true
    }
}

#[Swag.PrintBc]
#test
{
    var x: AA
    @assert(x == x)
}
