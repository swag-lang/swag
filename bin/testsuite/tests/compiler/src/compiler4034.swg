//#global testerror

//#[Swag.PrintBc]
#test
{
    var a = dref @typeof(s32)
    @assert(a.name == "s32")
}
