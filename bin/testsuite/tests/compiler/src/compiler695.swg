struct(T) Point
{
    x:     T = 5
    y:     T = 10
}

struct(V) Array
{
    pt:  Point'V
}

#test
{
    var x: Array's32
}
