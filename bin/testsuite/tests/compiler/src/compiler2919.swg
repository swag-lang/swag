struct AA
{
    yy   = false
}

impl AA
{
    func toto(me, v: AA = {true})
    {
        @assert(v.yy)
    }
}

#test
{
    var xxx: AA
    xxx.toto()
}
