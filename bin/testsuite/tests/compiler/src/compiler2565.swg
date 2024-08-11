#global testerror "Err0309"

struct ErrTiti{}

struct AA
{

}

impl AA
{
    func opEquals(self, other: AA)->bool
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