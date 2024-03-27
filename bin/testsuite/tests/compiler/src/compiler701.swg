
struct(T) Point
{
    x:     T = 5
    y:     T = 100
}

struct(T) Array
{
    pt: Point'T
}

//#[Swag.PrintBc]
#test
{
    var arr: Array's32
    @assert(@sizeof(arr) == 8)
}
