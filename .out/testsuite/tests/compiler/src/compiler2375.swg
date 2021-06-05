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
    func toto(x: T)
    {
    }

    func toto(x: s32)
    {
    }
}

//#[Swag.printbc]
#test
{
    var v: BB'AA
    var v1: AA
    BB.toto(v1)
}