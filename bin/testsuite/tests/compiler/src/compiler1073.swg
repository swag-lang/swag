#test
{
	@assert(G == 100)
}

var G: s32 = ?

#run
{
	G = 100
}

#drop
{
    @assert(G == 100)
}