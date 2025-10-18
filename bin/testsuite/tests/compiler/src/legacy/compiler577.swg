#global testerror "Err0705"

struct Point
{
    x:     s32
    y:     s32
}

impl Point
{
    func opEquals(me, other: Point) {}
}
