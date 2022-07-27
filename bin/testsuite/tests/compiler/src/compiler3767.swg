#global testerror

#test
{
    const A = @[1]
    ptr := &A[0]
    dref ptr = 1
}