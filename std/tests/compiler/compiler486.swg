


//#[swag.printbc]
#test
{
	var res = T()
	@assert(res.val0 == true)
	@assert(res.val1 == 2)
	@assert(res.val2 == false)
}

//#[swag.printbc]
func T()-> {bool,s32,bool}
{
	return {true,2,false}
}
