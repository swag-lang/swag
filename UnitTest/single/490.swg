#module module490
//#unittest pass semantic

//#[swag.printbc]
#test
{
	var res : {bool, bool} = {true, false}
	@assert(res.0 == true)
	@assert(res.1 == false)
}
