#test
{
    {
        var x = "abcd"
        var y = x[..1]
        @assert(y == "ab")
    }

    {
        var x = "abcd"
        var y = x[..<2]
        @assert(y == "ab")
    }
}
