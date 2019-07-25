#module module491
//#unittest pass semantic

//#[swag.printbytecode]
#test
{
	res := {"true", "false"}
	@assert(res.0 == "true")
	@assert(res.1 == "false")
}
