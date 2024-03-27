#global testerror "Err0042"
struct A { x: s32; }
impl A
{
    mtd(op: string) opBinary(value: s32)->Self
    {
        return {value}
    }
}

#test
{
    const C: A = {1}
    const D: s32 = 2
    //var v = C - D
    const E = C - D
    //@print(v.x)
}