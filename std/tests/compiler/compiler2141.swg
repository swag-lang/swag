// A simple point with 2 float coordinates X and Y
struct(T) PointBase
{
    x, y: T
}

impl PointBase
{
    // Offset this Point by a given value
    func offset(using self, value: T)
    {
        x, y += value
    }

    // Offset this Point by a given value
    func offset(using self, valueX, valueY: T)
    {
        x += valueX
        y += valueY
    }
}

#bake Point2141 = PointBase'f32