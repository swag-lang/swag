#test
{
    var X = [1, 2]
    var ptr = @dataof(X)
    ptr[0] = 666
    @assert(X[0] == 666)
}