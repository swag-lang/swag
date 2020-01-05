

func(T) delete(addr: *T)
{
	@assert(addr[0] == 666)
}

//#[swag.printbc]
#test
{
	x := 666
	delete's32(&x)
}