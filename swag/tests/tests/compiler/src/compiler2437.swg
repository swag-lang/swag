struct A
{
}

impl A
{
    func opEquals(using self, o: string)->bool
    {
        return o == "toto"
    }
}

//#[swag.printbc]
#test
{
    var x: A
    @assert("toto" == x)
}