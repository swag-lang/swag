#global skip
struct A {}
impl A
{
    mtd opIndex(x: string)->s32
    {
        @assert(x == "a")
        return 52
    }
}

#test
{
    var a: A
    let b = a["a"]
    @assert(b == 52)
}