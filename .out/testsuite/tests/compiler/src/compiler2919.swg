struct AA
{
    yy = false
}

impl AA
{
    func toto(using self, v: AA = @{true})
    {
        @assert(v.yy)
    }
}

#test
{
    var xxx: AA
    xxx.toto()
}