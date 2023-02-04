#global skip
struct Point
{
    x, y: s32
}

func origin() -> Point
{
    x := Point(x: 0, y: 0)
    return x
}

func toto()
{
    x := origin ()
}