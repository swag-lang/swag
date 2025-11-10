var G = 0's64

func print(f: string)
    verify
    {
        @assert(#isconstexpr(f))
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
    if @bytecode do
        @assert(G == 2)
}
