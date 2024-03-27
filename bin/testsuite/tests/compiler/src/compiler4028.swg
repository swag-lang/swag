#global testerror "Err0013"

struct Point
{
    x: s32
}

func toto(using pt: *Point, using pt1: *Point)
{
    x = 0
}

//#[Swag.PrintBc]
#test
{
}
