struct A
{
    x:   s32 = 555
}

var a: A
func toto()->*A
{
    return &a
}

impl A
{
    mtd titi()->s32
    {
        return me.x
    }
}

#test
{
    let t = toto().titi()
    @assert(t == 555)
}
