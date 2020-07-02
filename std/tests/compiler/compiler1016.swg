using swag

//#[swag.printbc]
#test
{
	ptr := @typeof(*TypeInfo)
    //@print(ptr.name)
	@assert(ptr.name == "*swag.TypeInfo")
}
