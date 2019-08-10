#module module587
#unittest pass semantic

var x = 1

func(op: string) toto()
{
	x = 2
}

#test
{
	toto!("op")()
	@assert(x == 2)
}