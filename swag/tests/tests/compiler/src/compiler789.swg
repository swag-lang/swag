

//#[swag.printbc]
#test
{
	pt := @["A"]
	var slice: [..] *string = @[&pt[0]]

	str := slice[0, 0]
	@assert(str == "A")

	str1 := slice[0]
	@assert(str1[0] == "A")
}