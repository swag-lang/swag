using Swag

struct AA
{

}

impl AA
{
    func opEquals(using me, other: string)->bool
    {
        return other == "toto"
    }

    #[Implicit]
    func opCast(using me)->string
    {
        return "toto"
    }
}

//#[Swag.PrintBc]
#test
{
    var v: AA
    var vs: AA
    @assert(v == vs)
}