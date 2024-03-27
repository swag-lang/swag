
struct Point
{
    x:     s32
    y:     s32
}

struct Pair
{
    x:     string
    y:     Point
}

//#[Swag.PrintBc]
#test
{
    var pair: Pair{"toto", {10, 20}}
    @assert(pair.y.x == 10)
    @assert(pair.y.y == 20)
}
