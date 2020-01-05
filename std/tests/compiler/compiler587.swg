


var x = 1

func(op: string) toto()
{
	x = 2
}

#test
{
	toto'("op")()
	@assert(x == 2)
}