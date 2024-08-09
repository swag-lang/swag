#global testerror "Err0622"

struct(T) A
{
}

impl A
{
    mtd toto(x: const [..] T)
    {
    }
}

#test
{
    var a: A'([2] s32)
    a.toto([1, 2])
}