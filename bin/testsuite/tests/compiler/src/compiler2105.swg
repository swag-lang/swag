struct AA
{
    x, y: f64

    enum BB
    {
        R
        G
    }

    z: BB = BB.R
}

#test
{
    var v: AA
    @assert(v.z == AA.BB.R)
}