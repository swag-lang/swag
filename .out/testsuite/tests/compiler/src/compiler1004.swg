#global testerror

using Swag

//#[Swag.PrintBc]
#test
{
	let x: any = 6
	@assert(cast(string) x == "6")
}
