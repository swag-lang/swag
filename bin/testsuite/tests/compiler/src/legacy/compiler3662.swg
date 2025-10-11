//#global skip
struct A { x: string }
impl A
{
    mtd(op: string) opBinary(value: string)->A
    {
        return {value}
    }
}

#test
{
    const C: A = {"1"}
    let v    = C << "2"
    @assert(v.x == "2")
}
