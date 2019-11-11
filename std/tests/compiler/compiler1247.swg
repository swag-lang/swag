using swag

func toto()
{
    cxt := @context()
    
    var req: AllocatorRequest
    req.size = 10
    cxt.allocator(&req)
}

#test
{
    toto()
    
    var cxt: Context
    cxt = deref @context()
}