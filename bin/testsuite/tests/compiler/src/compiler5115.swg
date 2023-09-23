#global testerror "Err0659"

struct A {}
impl A
{
    mtd(x) opBinary(b: s32)->Self
    {
        return {}
    }
}