struct(T) AA
{
}

impl AA
{
    func toto(using self)->{T}
    {
        alias RR = struct{T}
        return RR{50}
    }
}

//#[swag.printbc]
#test
{
    var x: AA's64   
    v := x.toto()
    @assert(v.item0 == 50)
}
