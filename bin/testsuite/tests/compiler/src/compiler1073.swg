#test
{
    @assert(G == 100)
}

var G: s32 = undefined

#run
{
    G = 100
}

#drop
{
    @assert(G == 100)
}
