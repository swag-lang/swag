struct(T) AA {}

impl AA
{
    func toto(me)->{ a: T }
    {
        alias RR = { a: T }
        return RR{50}
    }
}

//#[Swag.PrintBc]
#test
{
    var x: AA's64
    let v  = x.toto()
    @assert(v.item0 == 50)
}
