//#[Swag.PrintBc]
#test
{
    var x     = ["10", "20", "30"]
    var count = 1
    count -= 1
    @init(&x, count)("666")
    @assert(x[0] == "10")
    @assert(x[1] == "20")
    @assert(x[2] == "30")
}
