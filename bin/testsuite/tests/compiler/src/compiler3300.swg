struct Duration
{
    val: f32
}

impl Duration
{
    mtd(op: string) opAffectLiteral(v: f32)
    {
        #if op == "s" do
            me.val = v * 1000
        #elif op == "ms" do
            me.val = v
        #else do
            #assert(false)
    }
}

#test
{
    let x: Duration = 1.0's
    @assert(x.val == 1000.0)
    let x1: Duration = 1.0'ms
    @assert(x1.val == 1.0)
}
