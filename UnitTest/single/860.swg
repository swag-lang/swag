#module module860

//#[swag.printbc]
#test
{
	a := 0
	switch a
	{	
	case 0:
		//defer @print("0")
		a += 1
	case 1:
		//defer @print("1")
		a += 2
	}
	
	@assert(a == 1)
}