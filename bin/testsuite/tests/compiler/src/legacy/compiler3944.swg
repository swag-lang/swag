struct V { x, y: s32 }

impl V
{
    mtd opIndexAffect(idx: u64, val: s32)
    {
        me.x, me.y = val
    }
}

//#[Swag.PrintBc]
#test
{
    var x: V
    var y: &V = &x
    y[0] = 1
    @assert(y.x == 1)
    @assert(y.y == 1)
}
