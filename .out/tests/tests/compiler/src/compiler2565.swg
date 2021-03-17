#global testerror

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

#[swag.printbc]
#test
{
    var x: AA
    @assert(x == x)
}