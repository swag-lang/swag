#module module756

func toto(x: [..] s32)
{
	@assert(@op(x, count) == 2)
}

//#[swag.printbc]
#test
{	
	array := [1 , 2]
	toto(array)
}
