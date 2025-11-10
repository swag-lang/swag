using Swag

struct AA {}

impl AA
{
    func opEquals(me, other: string)->bool
    {
        return other == "toto"
    }

    #[Implicit]
    func opCast(me)->string
    {
        return "toto"
    }
}

//#[Swag.PrintBc]
#test
{
    var v:  AA
    var vs: AA
    @assert(v == vs)
}
