using Swag

//#[Swag.PrintBc]
#test
{
	ptr := @typeof(const *TypeInfo)
    //@print(ptr.name)
	@assert(ptr.name == "const *Swag.TypeInfo")
}
