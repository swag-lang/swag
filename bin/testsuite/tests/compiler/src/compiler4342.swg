struct(T) Point
{
    x:     T
    y:     T
}

#test
{
    var integer  = Point{x: 5's32, y: 10's32}
    var integer1 = Point{x: true, y: false}
}
