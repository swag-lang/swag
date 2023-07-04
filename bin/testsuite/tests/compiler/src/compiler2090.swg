struct(T) AA
{
}

impl AA
{
    func toto(using self)->{a: T}
    {
        return {5}
    }
}

//#[Swag.PrintBc]
#test
{
    var x: AA's64
    v := x.toto()
    @assert(v.item0 == 5)
}
