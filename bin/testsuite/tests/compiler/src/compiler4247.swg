#test
{
    var x = "abcd"
    var y = x[2..]
    @assert(y == "cd")
}

#test
{
    var x = "abcd"
    var y = x[0..]
    @assert(y == "abcd")
}
