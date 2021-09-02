//#[Swag.PrintBc]
#test
{
    var arr: [4] string = @["AAA", "123", "CCC", "DDD"]
    x := arr[1, 0]
    @assert(x == "1"'u8)
    x1 := arr[1, 1]
    @assert(x1 == "2"'u8)
}