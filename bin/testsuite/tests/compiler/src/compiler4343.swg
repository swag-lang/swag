#global testerror "Err0653"
struct(T) Point
{
    x: T
    y: T
}

func(T) toto (x: T) {}

#test
{
    var integer2 = Point{ x: true, y: 1.0'f32 }
}
