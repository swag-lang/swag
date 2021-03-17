/*#run
{
	@assert(G == 100)
}

var G: s32

#init
{
	G = 100
}

#drop
{
	G = 0
}*/