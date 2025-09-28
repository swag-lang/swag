//#[Swag.PrintBc]
#test
{
    var x = 10
    @init(&x, 1)(666)
    @assert(x == 666)

    var y = "10"
    @init(&y, 1)("666")
    @assert(y == "666")
}
