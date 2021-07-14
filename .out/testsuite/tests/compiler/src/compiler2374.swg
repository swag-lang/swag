using Swag

struct AA
{

}

impl AA
{
    func opEquals(using self, other: string)->bool
    {
        return other == "toto"
    }

    #[implicit]
    func opCast(using self)->string
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