//#[swag.printbc]
#test
{
	var res : struct{bool, bool} = @{true, false}
	@assert(res.item0 == true)
	@assert(res.item1 == false)
}
