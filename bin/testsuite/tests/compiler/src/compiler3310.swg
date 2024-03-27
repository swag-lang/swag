#global skip
//#global testerror
struct Duration
{
    val: f32
}

var G = 0

impl Duration
{
    mtd(suffix: string, T) opAffectLiteral(v: T)
    {
        #assert suffix == "s"
        #assert T == s32
        G = 1
    }
}

#test
{
    var x: Duration = 6's
    @assert(G == 1)
}
