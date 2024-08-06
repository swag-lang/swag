#global testerror "Err0600"
#test
{
    var x = [0, 2, 3, 4]
    var ptr = &x[0]

    @assert(x[1] == 2)
    dref (ptr + 1) = true
    @assert(x[1] == 1)
}