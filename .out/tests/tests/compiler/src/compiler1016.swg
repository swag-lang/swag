using swag

//#[swag.printbc]
#test
{
	ptr := @typeof(const *TypeInfo)
    //@print(ptr.name)
	@assert(ptr.name == "const *swag.TypeInfo")
}
