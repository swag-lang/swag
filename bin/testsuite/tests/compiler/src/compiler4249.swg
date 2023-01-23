#test
{
    {
        x := "abcd"
        y := x[..1]
        @assert(y == "ab")
    }

    {
        x := "abcd"
        y := x[..<2]
        @assert(y == "ab")
    }
}
