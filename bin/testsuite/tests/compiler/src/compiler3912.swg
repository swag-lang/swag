//#global skip

struct(T) A {}

impl A
{
    mtd toto(x: const [2] T)
    {
        @assert(x[0] == 1)
        @assert(x[1] == 2)
    }
}

#test
{
    //var a: [2] [..] s32
    var a: A'(s32)
    a.toto([1, 2])
}
