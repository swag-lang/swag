
using swag

//#[swag.printbc]
#test
{
	ptr := @typeof(*TypeInfo)
	@assert(ptr.name == "*TypeInfo")
}
