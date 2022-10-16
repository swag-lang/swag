#global skip

struct(T) A
{
}

impl A
{
    mtd toto(x: [2] T)
    {
    }
}

#test
{
    var a: A'([2] s32)
    a.toto(@[1, 2])
}