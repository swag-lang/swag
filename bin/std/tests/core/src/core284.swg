using Core

func toto()->Array's32
{
    var result: retval
    result.add(1)
    result.add(2)
    result.add(3)
    result.add(4)
    return result
}

const X = #run toto()
#test
{
    @assert(@countof(X) == 4)
    @assert(X[0] == 1)
    @assert(X[1] == 2)
    @assert(X[2] == 3)
    @assert(X[3] == 4)
}