//#global skip
struct A { x: s32; }
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
    var v = C << 2
    @assert(v.x == 2)
}