var G = 0's64

func print(f: string)
    where,call
    {
        @assert(@isconstexpr(f))
        @assert(f == "tt()" or f == "tt1()")
        G += 1
        return true
    }
{

}

#test
{
    print("tt()")
    print("tt1()")
    if @isbytecode() do @assert(G == 2)
}
