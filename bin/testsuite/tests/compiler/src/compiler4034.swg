//#global testerror

//#[Swag.PrintBc]
#test
{
	a := dref @typeof(s32)
    @assert(a.name == "s32")
}
