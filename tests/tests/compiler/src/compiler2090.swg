struct(T) AA
{
}

impl AA
{
    func toto(using self)->{T}
    {
        return @{5}
    }
}

//#[swag.printbc]
#test
{
    var x: AA's64   
    v := x.toto()
    @assert(v.item0 == 5)
}
