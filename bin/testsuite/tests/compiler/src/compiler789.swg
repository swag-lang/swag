

//#[Swag.PrintBc]
#test
{
	var pt = ["A"]
	var slice: [..] ^string = [&pt[0]]

	var str = slice[0, 0]
	@assert(str == "A")

	var str1 = slice[0]
	@assert(str1[0] == "A")
}