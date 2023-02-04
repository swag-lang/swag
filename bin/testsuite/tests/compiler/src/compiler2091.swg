struct(T) AA
{
}

impl AA
{
    func toto(using self)->{T}
    {
        alias RR = {T}
        return RR{50}
    }
}

//#[Swag.PrintBc]
#test
{
    var x: AA's64
    v := x.toto()
    @assert(v.item0 == 50)
}
