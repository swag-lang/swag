struct(K, V) A {}
impl A
{
    mtd opAffect(slice: const [..] {k: K, v: V})
    {
        visit v: slice
        {
            @assert(v.k == 1)
            @assert(v.v == 2)
        }
    }
}

//#[Swag.PrintBc]
#test
{
    var a: A'(s32, s32)
    a = [{1, 2}]
}