#global testerror "Err0649"

//#[Swag.PrintBc]
struct Point
{
    x:     f32
    y:     f32
}

//#[Swag.PrintBc]
func Print(x: s64)
{
}

//#[Swag.PrintBc]
#test
{
    Print(s32)
}
