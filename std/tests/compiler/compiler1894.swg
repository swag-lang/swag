#test
{
    test()
}

func test(x = #callerloc)
{
    //@print(x.file)
    @assert(x.lineStart == 3)
    @assert(x.lineEnd == 3)
    @assert(x.colStart == 5)
}