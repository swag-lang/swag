#test
{
    test()
}

func test(x = #callerlocation)
{
    //@print(x.fileName)
    @assert(x.lineStart == 3)
    @assert(x.lineEnd == 3)
    @assert(x.colStart == 5)
}