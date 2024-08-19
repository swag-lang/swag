#global testerror "Err0486"
struct A
{
}

struct B {}

impl B
{
    func toto(x: s32)
    {
    }
}

impl A
{
    mtd titi(using ptr: *B)
    {
        ptr.toto(2)
    }
}