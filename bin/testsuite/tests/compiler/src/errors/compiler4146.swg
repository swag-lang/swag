#global testerror "Err0641"
struct A {}

struct B {}

impl B
{
    func toto(x: s32) {}
}

impl A
{
    mtd titi(ptr: *B)
    {
        ptr.toto(2)
    }
}
