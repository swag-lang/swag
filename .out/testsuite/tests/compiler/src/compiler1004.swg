#global testerror

using Swag

//#[Swag.printbc]
#test
{
	let x: any = 6
	@assert(cast(string) x == "6")
}
