struct Point1
{
    x:   s32
}

struct Point
{
    x:      s32 = undefined
    y:      s32 = undefined
    pt:     Point1
}

//#[Swag.PrintBc]
#test
{
    var pt: Point
}
