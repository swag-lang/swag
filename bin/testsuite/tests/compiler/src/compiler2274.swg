var g = #load(#file)

#test
{
    @assert(g[0] == "v"'u8)
    @assert(g[1] == "a"'u8)
    @assert(g[2] == "r"'u8)
    //@print(cast(string) g)
    g[0] = 1
}