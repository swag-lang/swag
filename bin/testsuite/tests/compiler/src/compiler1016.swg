using Swag

//#[Swag.PrintBc]
#test
{
	var ptr = @typeof(const *TypeInfo)
    //@print(ptr.name)
	@assert(ptr.fullname == "const *Swag.TypeInfo")
}
