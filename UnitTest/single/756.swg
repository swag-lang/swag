#module module756

func toto(x: const [..] s32)
{
	@assert(@op(x, count) == 2)
}

//#[swag.printbc]
#test
{	
	array := [1 , 2]
	toto(array)
	toto([4, 5])
}
