struct A
{
    x:   [3] s32
}

impl A
{
    #[Swag.Inline]
    mtd(op: string) opIndexAssign(index0: u64, value: s32)
    {
        #ast "me.x[index0] " ++ op ++ " value"
    }
}

#test
{
    var a: A
    a.x[1] = 5
    a[1] += 661
    @assert(a.x[1] == 666)
}
