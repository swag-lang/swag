#global testerror "Err0137"

struct AA
{

}

impl AA
{
    func opEquals(self, other: AA)->bool
    {
        throw "toto"
        return true
    }
}

#[Swag.PrintBc]
#test
{
    var x: AA
    @assert(x == x)
}