struct(T) A
{
}

var G: u32 = 0

impl A
{
    func opIndexAffect(using self, idx: u64, v: T)
    {
        G += 1
    }
}

#test
{
    G = 0
    var x: A'(*u8)
    loop i in "toto":
        x[i] = null
    @assert(G == 4)
}
