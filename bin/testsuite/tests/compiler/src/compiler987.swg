struct Point
{
    x:   s32
}

//#[Swag.PrintBc]
#test
{
    var pair = {"toto", Point{10}}
    @assert(pair.item1.x == 10)
}
