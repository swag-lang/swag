#test
{
    let x = "abcd"
    let y = x[2 to ]
    @assert(y == "cd")
}

#test
{
    let x = "abcd"
    let y = x[0 to ]
    @assert(y == "abcd")
}
