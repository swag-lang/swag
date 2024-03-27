//#global skip
struct A
{
    x: string
}
impl A
{
    mtd(op: string) opBinary(value: string)->Self
    {
        return {value}
    }
}

#test
{
    const C: A = {"1"}
    var v    = C << "2"
    @assert(v.x == "2")
}
