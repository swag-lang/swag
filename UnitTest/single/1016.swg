#module module1016
using swag

//#[swag.printbc]
#test
{
	ptr := @typeof(*Typeinfo)
	@assert(ptr.name == "*Typeinfo")
}
