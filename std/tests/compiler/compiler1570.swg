struct Point
{
    x: s64 = 1
}

func toto(val: *Point)
{
    var y: Point = val
}

#test
{
    var value: Point
    value.x = 666
    toto(value)
    @print(value.x)
}
