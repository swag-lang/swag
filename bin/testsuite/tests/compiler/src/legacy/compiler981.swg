#global testerror "Err0510"

struct Point
{
    x:     s32
    y:     s32
}

const NullPoint: Point{10, 50} = {100, 500}

//#[Swag.PrintBc]
#test
{
    @assert(NullPoint.x == 100)
    @assert(NullPoint.y == 500)
}
