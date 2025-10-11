//#global skip
struct Point { x, y: s32 }

impl Point
{
    const Zero = Point{0, 0}
}

struct(T) Array {}

impl Array
{
    mtd add(val: T) {}
}

#test
{
    var arr: Array'Point
    arr.add(Point.Zero)
}
