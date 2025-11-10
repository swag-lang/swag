//#global skip
struct A { x: s32 }
impl A
{
    mtd(op: string) opBinary(value: s32)->A
    {
        return {value}
    }
}

#test
{
    const C: A = {1}
    const D: s32 = 2
    let v    = C - D
    @assert(v.x == 2)
}
