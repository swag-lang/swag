#global testerror "Err0015"
struct(T) Point
{
    x: T
    y: T
}

func(T) toto (x: T) {}

#test
{
    var integer2 = Point{ x: 1, y: 1 }
}
