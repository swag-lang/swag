#test
{
    var x   = [0, 2, 3, 4]
    let ptr = &x[0]

    @assert(x[1] == 2)
    dref (ptr + 1) = 1
    @assert(x[1] == 1)
}
