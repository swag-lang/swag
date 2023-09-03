#global testerror "Saf0002"

using Swag

//#[Swag.PrintBc]
#test
{
	const x: any = 6
    @assert(cast(string) x == 6)
	//@assert(cast(string) x == "6")
}
