

//#[Swag.printbc]
#test
{
	var a = true ? "1" : "10"
	@assert(a == "1")
	var b = false ? "1" : "10"
	@assert(b == "10")
}
