#module module789

//#[swag.printbc]
#test
{
	pt := ["A"]
	var slice: [..] *string = [&pt[0]]
	
	str := slice[0, 0]
	@assert(str == "A")
}