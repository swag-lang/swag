
struct Point
{
    x:     s32
    y:     s32
}

struct Pair
{
    x:     string
    y:     f32        //Point
}

//#[Swag.PrintBc]
#test
{
    //var pair: Pair("toto", {10,20})
    var pair: Pair{"toto", 1}
    @assert(pair.x == "toto")
    @assert(pair.y == 1.0)
}
