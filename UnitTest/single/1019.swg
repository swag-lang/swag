#module module1019
using swag

func swag_Toto()
{
}

//#[swag.printbc]
#test
{
	ptr := @getcontext()
	
	var req: AllocatorRequest
	req.size = 16
	
	ptr.allocator(&req)
	req.mode = AllocatorMode.Free
	ptr.allocator(&req)
}