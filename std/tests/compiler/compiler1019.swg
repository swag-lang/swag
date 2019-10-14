
using swag

//#[swag.printbc]
#test
{
	context := @context()
	
	var req: AllocatorRequest
	req.size = 16
	
	context.allocator(&req)
	req.mode = AllocatorMode.Free
	context.allocator(&req)
}