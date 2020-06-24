struct(T) A
{
}

var G: u32 = 0

impl A
{
    func opIndexAffect(using self, v: T, idx: u32)
    {
        G += 1
    }
}

#test
{
    G = 0
    var x: A'(*u8)
    loop i: "toto"
        x[i] = null
    @assert(G == 4)
}
