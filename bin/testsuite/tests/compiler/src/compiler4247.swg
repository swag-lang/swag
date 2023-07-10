#test
{
    let x = "abcd"
    let y = x[2..]
    @assert(y == "cd")
}

#test
{
    let x = "abcd"
    let y = x[0..]
    @assert(y == "abcd")
}
