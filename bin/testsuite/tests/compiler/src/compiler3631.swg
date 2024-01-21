#global testerror "Err0258"
struct Wnd
{
    x: s32
}

impl Wnd
{
    mtd toto()
    {
        var v: **s32
        v.x = 0
    }
}