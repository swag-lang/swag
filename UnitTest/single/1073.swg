#module module1073

#test
{
	@assert(G == 100)
	G = 0
}

var G: s32

#init
{
	G = 100
}