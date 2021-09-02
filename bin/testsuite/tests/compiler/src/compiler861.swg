

//#[Swag.PrintBc]
#test
{
	a := 30
	switch a
	{
	case 0:
		//defer @print("0")
		defer a += 1
		a = 0
	case 1:
		//defer @print("1")
		defer a += 2
		a = 0
	default:
		defer a += 3
		a = 0
	}

	@assert(a == 3)
}