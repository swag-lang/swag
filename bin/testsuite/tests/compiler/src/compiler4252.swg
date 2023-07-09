#test
{
    var x = "abcd"
    var v = 2
    var y = x[..<v]
    @assert(y == "ab")
}
