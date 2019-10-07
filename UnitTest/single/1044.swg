#module module1044

func new() => {1, 2}

//#[swag.printbc]
#test
{
	a := new().val0
	@assert(a == 1)
	b := new().val1
	@assert(b == 2)
	
	@assert(new().val0 == 1)
	@assert(new().val1 == 2)
}