#global testerror
struct V {}

impl V
{
    mtd opCount() => 5
    mtd opSlice(l, u: uint)->string
    {
        return "toto"
    }
}

#test
{
    var v: V
    y := v[0..]
}
