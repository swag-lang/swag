#global skip
struct Point
{
    x, y: s32
}

func origin() -> Point
{
    x := Point {0, 0}
    return x
}

func toto()
{
    v := Point
    (a, b) := origin()
}