#global testerror "Err0093"

#test
{
    const A = [1]
    var ptr = &A[0]
    dref ptr = 1
}