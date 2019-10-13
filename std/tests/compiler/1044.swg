

func new() => {1, 2}

//#[swag.printbc]
#test
{
	a := new()
	/*@assert(a.val0 == 1)
	@assert(a.val1 == 2)*/
	
	/*
	@assert(new().val0 == 1)
	@assert(new().val1 == 2)*/
}