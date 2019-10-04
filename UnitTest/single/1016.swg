#module module1016
using swag

//#[swag.printbc]
#test
{
	ptr := @typeof(*typeinfo)
	@assert(ptr.name == "*typeinfo")
}
