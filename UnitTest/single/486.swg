#module module486
//#unittest pass semantic

//#[swag.printbytecode]
#test
{
	var res = T()
	@assert(res.0 == true)
	@assert(res.1 == 2)
	@assert(res.2 == false)
}

//#[swag.printbytecode]
func T()-> {bool,s32,bool}
{
	return {true,2,false}
}
