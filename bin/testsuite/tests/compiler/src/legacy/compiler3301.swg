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
    var x: Duration
    x = 1.0's
    @assert(x.val == 1000.0)
}
