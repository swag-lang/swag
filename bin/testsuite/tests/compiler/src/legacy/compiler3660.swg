//#global skip
struct A { x: u32 }
impl A
{
    mtd(op: string) opBinary(value: u32)->A
    {
        return {value}
    }
}

#test
{
    const C: A = {1}
    const D: s32 = 2
    let v    = C << 2
    @assert(v.x == 2)
}
