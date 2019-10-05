#module module1019
using swag

//#[swag.printbc]
#test
{
	ptr := @getcontext()
	
	addr := ptr.allocator(16, AllocatorMode.Alloc, null)
	ptr.allocator(0, AllocatorMode.Free, addr)
}