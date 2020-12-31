struct AA
{

}

impl AA
{
    func opEquals(using self, other: string)->bool
    {
        return other == "toto"
    }

    func opCast(using self)->string
    {
        return "toto"
    }
}

//#[swag.printbc]
#test
{
    var v: AA
    var vs: AA
    @assert(v == vs)
}