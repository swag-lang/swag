var G = 0's64

func print(f: string)
    #selectif
    {
        @assert(@isconstexpr(f))
        @assert(f == "tt()" || f == "tt1()")
        G += 1
        return true
    }
{

}

#test
{
    print("tt()")
    print("tt1()")
    if @isbytecode() @assert(G == 2)
}
