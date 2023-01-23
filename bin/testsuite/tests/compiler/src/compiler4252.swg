#test
{
    x := "abcd"
    v := 2
    y := x[..<v]
    @assert(y == "ab")
}
