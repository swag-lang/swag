#test
{
    const x = "abcd"
    let y   = x[2 to ]
    @assert(y == "cd")
}

#test
{
    const x = [1, 2, 3, 4]
    let y   = x[2 to ]
    @assert(y[0] == 3)
    @assert(y[1] == 4)
}
