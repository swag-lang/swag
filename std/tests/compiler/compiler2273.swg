const g = #load(#file)

#test
{
    @assert(g[0] == "c"'u8)
    @assert(g[1] == "o"'u8)
    @assert(g[2] == "n"'u8)
    @assert(g[3] == "s"'u8)
    @assert(g[4] == "t"'u8)
    //@print(cast(string) g)
}