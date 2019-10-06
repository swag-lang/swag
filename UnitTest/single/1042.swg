#module module1042

func(T) new()->*T
{
	var req : swag.AllocatorRequest
	req.size = @sizeof(T)
	
	context := @getcontext() 
	context.allocator(req)
	
	init(cast(*T) req.address)
	return acast req.address
}

//#[swag.printbc]
#test
{
	ptr := new!s32()
}