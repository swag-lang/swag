#test
{
    const x = "abcd"
    var y = x[2..]
    @assert(y == "cd")
}

#test
{
    const x = [1,2,3,4]
    var y = x[2..]
    @assert(y[0] == 3)
    @assert(y[1] == 4)
}