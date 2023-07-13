#global testerror

using Swag

//#[Swag.PrintBc]
#test
{
	const x: any = 6
    @assert(cast(string) x == 6)
	//@assert(cast(string) x == "6")
}
