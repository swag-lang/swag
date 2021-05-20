struct AA
{
    y = false
}

struct BB
{
    yy = 666
}

impl AA
{
    func toto(using self, v: BB = @{})
    {
        @assert(v.yy == 666)
    }
}

//#[Swag.printbc]
#test
{
    var xxx: AA
    xxx.toto()
}