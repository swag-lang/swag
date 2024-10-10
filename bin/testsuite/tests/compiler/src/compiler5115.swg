#global testerror "Err0195"

struct A {}
impl A
{
    mtd(x) opBinary(b: s32)->Self
    {
        return {}
    }
}