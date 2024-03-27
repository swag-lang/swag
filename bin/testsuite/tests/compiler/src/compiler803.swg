#global testerror "Err0106"

struct Point
{
    x:     s32
    y:     s32
}

//#[Swag.PrintBc]
func toto(using pt: Point)
{
    x = 10
    y = 20
}

//#[Swag.PrintBc]
#test
{
}
