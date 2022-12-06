#global skip

struct(T) A
{
}

impl A
{
    mtd toto(x: [..] T)
    {
    }
}

#test
{
    var a: A'([..] s32)
    a.toto([1, 2])
}