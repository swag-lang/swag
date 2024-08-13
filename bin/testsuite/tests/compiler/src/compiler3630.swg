#global testerror "Err0157"

struct Wnd
{
    x: s32
}

var childs: [4] *Wnd

impl Wnd
{
    mtd toto()
    {
        visit &c: childs
        {
            switch c.x
            {
            case 0:
                break
            }
        }
    }
}