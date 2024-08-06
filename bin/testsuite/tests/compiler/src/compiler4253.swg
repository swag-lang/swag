#global testerror "Err0599"
struct V {}

impl V
{
    mtd opCount() => 5
    mtd opSlice(l, u: u64)->string
    {
        return "toto"
    }
}

#test
{
    var v: V
    var y = v[0..]
}
