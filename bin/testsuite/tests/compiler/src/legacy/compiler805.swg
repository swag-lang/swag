#global testerror "Err0699"

struct Point
{
    x:     s32
    y:     s32
}

//#[Swag.PrintBc]
func toto(pt: **Point)
{
    pt.x = 10
    pt.y = 20
}
