#test
{
    var x = "x"
    var y = &x
    dref y = "y"
    @assert(x == "y")
}
