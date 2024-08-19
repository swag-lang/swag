#global testerror "Err0323"

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