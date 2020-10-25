// A simple point with 2 float coordinates X and Y
struct(T) Point
{
    x, y: T
}

impl Point
{
    // Offset this Point by a given value
    #if @typeof(T) == bool
    {
        func offset(using self)
        {
        }
    }
}

#test
{
    var x: Point'f32
    //x.z = true
    //x.offset()
}