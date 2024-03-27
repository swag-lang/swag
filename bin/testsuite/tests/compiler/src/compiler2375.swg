struct AA
{
}

impl AA
{
    func opCast(using self)->string
    {
        return "toto"
    }
}

struct(T) BB
{
}

impl BB
{
    #[Swag.Overload]
    func toto(x: T)
    {
    }

    #[Swag.Overload]
    func toto(x: s32)
    {
    }
}

//#[Swag.PrintBc]
#test
{
    var v:  BB'AA
    var v1: AA
    BB.toto(v1)
}
