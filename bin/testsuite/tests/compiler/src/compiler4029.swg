#global testerror "Err0093"

struct Point
{
	x: s32
}

//#[Swag.PrintBc]
#test
{
    const X: Point
    with X
    {
        .x = 0
    }
}