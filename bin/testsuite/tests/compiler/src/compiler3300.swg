struct Duration
{
    val: f32
}

impl Duration
{
    mtd(op: string) opAffectLiteral(v: f32)
    {
        #if op == "s":
            val = v * 1000
        #elif op == "ms":
            val = v
        #else:
            #assert(false)
    }
}

#test
{
    var x:  Duration = 1.0's
    @assert(x.val == 1000.0)
    var x1: Duration = 1.0'ms
    @assert(x1.val == 1.0)
}