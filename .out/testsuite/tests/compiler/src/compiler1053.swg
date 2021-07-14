

func(T) delete(addr: *T)
{
	@assert(addr[0] == 666)
}

//#[Swag.PrintBc]
#test
{
	x := 666
	delete's32(&x)
}