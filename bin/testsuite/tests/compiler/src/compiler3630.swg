#global testerror "Err0171"

struct Wnd
{
    x: s32
}

var childs: [4] *Wnd

impl Wnd
{
    mtd toto()
    {
        foreach &c in childs
        {
            switch c.x
            {
            case 0:
                break
            }
        }
    }
}