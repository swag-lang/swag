#global testerror "Err0020"

#test
{
    const A = [1]
    var ptr = &A[0]
    dref ptr = 1
}
