#global testerror "Err0634"

struct A {}
impl A
{
    mtd(x) opBinary(b: s32)->A
    {
        return {}
    }
}
