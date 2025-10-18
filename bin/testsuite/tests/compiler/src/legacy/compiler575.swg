#global testerror "Err0743"

struct Point
{
    x:     s32
    y:     s32
}

impl Point
{
    func opEquals(me, other: Point, x: s32)->bool
    {
        return me.x == other.x and me.y == other.y
    }
}
