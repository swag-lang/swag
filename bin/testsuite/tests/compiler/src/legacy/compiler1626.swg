struct(T) A {}

var G: u32 = 0

impl A
{
    func opIndexAffect(me, idx: u64, v: T)
    {
        G += 1
    }
}

#test
{
    G = 0
    var x: A'(*u8)
    for i in "toto" do
        x[i] = null
    @assert(G == 4)
}
