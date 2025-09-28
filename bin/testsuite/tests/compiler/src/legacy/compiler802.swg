#global testerror "Err0091"

struct Point
{
    x:   s32
}

func toto(pt: Point, pt1: Point)
{
    pt.x = 0
}

//#[Swag.PrintBc]
#test
{
}
