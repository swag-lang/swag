#global testerror

using swag

//#[swag.printbc]
#test
{
	let x: any = 6
	@assert(cast(string) x == "6")
}
