//#global testerror "Err0344"

struct Point
{
    x:     s32
    y:     s32
}

impl Point
{
}

//#[Swag.PrintBc]
#test
{
    var pt1: Point
    var pt2: Point
    @assert(pt1 == pt2)
    pt1.x = 1
    @assert(pt1 != pt2)
}
