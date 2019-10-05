#module module1019
using swag

//#[swag.printbc]
#test
{
	ptr := @getcontext()
	ptr.allocator("toto")
	//@print(cast(s32) ptr.toto)
}