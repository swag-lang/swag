#global testerror "Err0511"
struct Point {}
impl Point
{
    func opEqual(me, other: Point)->bool
    {
        return me.x == other.x and me.y == other.y
    }
}
