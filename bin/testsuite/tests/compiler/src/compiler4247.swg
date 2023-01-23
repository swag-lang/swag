#test
{
    x := "abcd"
    y := x[2..]
    @assert(y == "cd")
}

#test
{
    x := "abcd"
    y := x[0..]
    @assert(y == "abcd")
}
