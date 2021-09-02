struct AA
{
    x, y: f64
    z: BB = BB.R
}

impl AA
{
    enum BB
    {
        R
        G
    }
}

#test
{
    var v: AA
    @assert(v.z == AA.BB.R)
}