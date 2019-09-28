#module module490
//#unittest pass semantic

//#[swag.printbc]
#test
{
	var res : {bool, bool} = {true, false}
	@assert(res.val0 == true)
	@assert(res.val1 == false)
}
