struct(T) AA
{
}

impl AA
{
    func toto(using self)->{a: T}
    {
        alias RR = {a: T}
        return RR{50}
    }
}

//#[Swag.PrintBc]
#test
{
    var x: AA's64
    var v = x.toto()
    @assert(v.item0 == 50)
}
