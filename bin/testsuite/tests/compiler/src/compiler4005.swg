#test
{
    x := "x"
    y := &x
    dref y = "y"
    @assert(x == "y")
}