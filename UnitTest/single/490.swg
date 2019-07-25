#module module490
//#unittest pass semantic

//#[swag.printbytecode]
#test
{
	var res : {bool, bool} = {true, false}
	@assert(res.0 == true)
	@assert(res.1 == false)
}
