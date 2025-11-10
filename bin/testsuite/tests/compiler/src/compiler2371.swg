#global skip
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
    func opIndex(me, idx: u64)->const &BB
    {
        return v
    }
}

impl BB
{
    func toto(const me)
    {
        @assert(v == 555)
    }
}

#test
{
    var v: AA
    v[0].toto()
}