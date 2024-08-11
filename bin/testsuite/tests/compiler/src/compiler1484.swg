#global testerror "Err0090"

#test
{
    const X = [1, 2]
    var ptr = @dataof(X)
    ptr[0] = 0
}