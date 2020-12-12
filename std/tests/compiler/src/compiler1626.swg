struct(T) A
{
}

var G: u32 = 0

impl A
{
    func opIndexAffect(using self, idx: u32, v: T)
    {
        G += 1
    }
}

#test
{
    G = 0
    var x: A'(*u8)
    loop i: "toto"
        x[cast(u32) i] = null
    @assert(G == 4)
}
