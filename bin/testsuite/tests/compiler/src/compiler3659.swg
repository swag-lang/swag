//#global skip
struct A { x: s32; }
impl A
{
    mtd(op: string) opUnary()->Self
    {
        return {2}
    }
}

#test
{
    const C: A = {1}
    const D: s32 = 2
    var v = -C
    @assert(v.x == 2)
}