struct AA
{
    v: BB
}

struct BB
{
    v: s64 = 555
}

impl AA
{
    func opIndex(using self, idx: uint)->const &BB
    {
        return v
    }
}

impl BB
{
    func toto(using const self)
    {
        @assert(v == 555)
    }
}

#test
{
    var v: AA
    v[0].toto()
}